#pragma once

#include <functional>
#include <atomic>

// TODO: add an allocator
// TODO: support weak pointers

// Observers:
// TODO: operator[]

namespace details
{

template<typename T>
using DeleterFunc = std::function<void(T*)>;
using CounterType = std::size_t;

template<typename T, typename Deleter = DeleterFunc<T>>
struct ControlBlock
{
    Deleter _deleter{};
    std::atomic<CounterType> _sharedCounter{};
    std::atomic<CounterType> _weakCounter{};
    T *_obj{};
};

template<typename T, typename Deleter = DeleterFunc<T>>
CounterType use_count(const ControlBlock<T, Deleter>* controlBlock)
{
    return controlBlock ? controlBlock->_sharedCounter.load() : 0;
}

template<typename T, typename Deleter = DeleterFunc<T>>
CounterType weak_use_count(const ControlBlock<T, Deleter>* controlBlock)
{
    return controlBlock ? controlBlock->_weakCounter.load() : 0;
}

template<typename T, typename Deleter = DeleterFunc<T>>
void incrementWeak(ControlBlock<T, Deleter>* controlBlock)
{
    if (weak_use_count(controlBlock)) {
        ++controlBlock->_weakCounter;
    }
}

template<typename T, typename Deleter = DeleterFunc<T>>
void decrementWeak(ControlBlock<T, Deleter>* controlBlock)
{
    if (weak_use_count(controlBlock)) {
        --controlBlock->_weakCounter;
    }
}

template<typename T, typename Deleter = DeleterFunc<T>>
void increment(ControlBlock<T, Deleter>* controlBlock)
{
    if (use_count(controlBlock)) {
        ++controlBlock->_sharedCounter;
    }
    incrementWeak(controlBlock);
}
template<typename T, typename Deleter = DeleterFunc<T>>
void decrement(ControlBlock<T, Deleter>* controlBlock)
{
    if (use_count(controlBlock)) {
        --controlBlock->_sharedCounter;
    }
    decrementWeak(controlBlock);
}

template<typename T, typename Deleter = DeleterFunc<T>>
details::ControlBlock<T, Deleter>* createControlBlock(T* obj, Deleter deleter)
{
    const auto& exist = obj != nullptr;
    return new ControlBlock<T, Deleter>{std::move(deleter), exist, exist, obj};
}

template<typename T, typename Deleter = DeleterFunc<T>>
DeleterFunc<T> createDeleter()
{
    return [](T* toDelete){ delete toDelete; };
}

} // namespace details

template<typename T, typename Deleter = details::DeleterFunc<T>>
class SharedPointer
{
public:
    SharedPointer() = default;

    explicit SharedPointer(T* obj)
            : SharedPointer(obj, details::createDeleter<T, Deleter>())
    {
    }
    SharedPointer(T* obj, Deleter deleter)
            : _controlBlock{details::createControlBlock(obj, std::move(deleter))}
    {
    }
    SharedPointer(const SharedPointer& rhs)
            : _controlBlock{rhs._controlBlock}
    {
        details::increment(_controlBlock);
    }
    SharedPointer(SharedPointer&& rhs) noexcept
            : _controlBlock{rhs._controlBlock}
    {
        rhs._controlBlock = nullptr;
    }
    ~SharedPointer()
    {
        clear();
    }

    SharedPointer& operator=(const SharedPointer& rhs)
    {
        if (this != &rhs) {
            clear();

            _controlBlock = rhs._controlBlock;
            details::increment(_controlBlock);
        }

        return *this;
    }
    SharedPointer& operator=(SharedPointer&& rhs) noexcept
    {
        if (this != &rhs) {
            swap(rhs);
        }
        return *this;
    }

    T* operator->() const noexcept
    {
        return get();
    }
    T& operator*() const noexcept
    {
        return *get();
    }
    explicit operator bool() const noexcept
    {
        return use_count();
    }

    T* get() const noexcept
    {
        return _controlBlock ? _controlBlock->_obj : nullptr;
    }
    std::size_t use_count() const noexcept
    {
        return details::use_count(_controlBlock);
    }
    void clear()
    {
        details::decrement(_controlBlock);

        // destroy obj
        if (_controlBlock && _controlBlock->_sharedCounter == 0) {
            _controlBlock->_deleter(_controlBlock->_obj);
        }

        // destroy controlBlock
        if (_controlBlock && _controlBlock->_weakCounter == 0) {
            delete _controlBlock; _controlBlock = nullptr;
        }
    }
    void reset(T* obj = nullptr, Deleter deleter = details::createDeleter<T, Deleter>())
    {
        clear();

        _controlBlock = createControlBlock(obj, std::move(deleter));
    }
    void swap(SharedPointer& rhs)
    {
        std::swap(_controlBlock, rhs._controlBlock);
    }
private:
    template<typename Y>
    friend class WeakPointer;

    details::ControlBlock<T, Deleter>* _controlBlock{};
};

// Description: Exists to track(non-own) an object, or to break cyclic references.
//              To use an object, pointed by the WeakPointer, you need to cast it to the shared pointer.

// API:
// Basic
// constructor
// destructor
// operator= assigns the weak_ptr

// Modifiers
// reset: releases the ownership of the managed object
// swap: swaps the managed objects

// Observers
// use_count: returns the number of shared_ptr objects that manage the object
// expired: checks whether the referenced object was already deleted
// lock: creates a shared_ptr that manages the referenced object
// owner_before: provides owner-based ordering of weak pointers

// Problem: How to manage the controlBlock?
//          How to do it, if it's an implementation detail for both, separate classes?
//          Isn't it, then, a common thing, which needs to be shared somehow?
//          And how? For example, you aren't able to do a 'is-a' relationship, because WeakPointer isn't allowed to have the same API.
//          And you can't copy the controlBlock from SharedPointer, because it's in the private scope.
// Solution: Mark as friend WeakPointer class for SharedPointer's controlBlock field, hence, have an access to control it too.
template<typename T>
class WeakPointer
{
public:
    WeakPointer(const WeakPointer<T>& rhs)
    {
    }

    WeakPointer(const SharedPointer<T>& rhs)
            : _controlBlock{rhs._controlBlock}
    {
    }

    ~WeakPointer()
    {
    }

private:
    details::ControlBlock<T>* _controlBlock;
};
