#pragma once

#include <functional>
#include <atomic>

// TODO: add an allocator
// TODO: support weak pointers

// Observers:
// TODO: operator[]

namespace utils
{

template<typename T>
using DeleterFunc = std::function<void(T*)>;

template<typename T, typename Deleter = DeleterFunc<T>>
struct ControlBlock
{
    Deleter _deleter{};
    std::atomic<std::size_t> _counter{};
    T *_obj{};
};

}

template<typename T, typename Deleter = utils::DeleterFunc<T>>
class SharedPointer
{
public:
    explicit SharedPointer(T* obj)
            : SharedPointer(obj, createDeleter())
    {
    }

    SharedPointer(T* obj, Deleter deleter)
            : _controlBlock{createControlBlock(obj, std::move(deleter))}
    {
    }

    SharedPointer(const SharedPointer& rhs)
            : _controlBlock{rhs._controlBlock}
    {
        increment();
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
            increment();
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

    T* get() const noexcept
    {
        return _controlBlock ? _controlBlock->_obj : nullptr;
    }

    T* operator->() const noexcept
    {
        return get();
    }

    // TODO: fix this potential explosion
    T& operator*() const noexcept
    {
        return *get();
    }

    explicit operator bool() const noexcept
    {
        return use_count();
    }

    std::size_t use_count() const noexcept
    {
        return _controlBlock ? _controlBlock->_counter.load() : 0;
    }

    void clear()
    {
        decrement();
        if (_controlBlock && _controlBlock->_counter == 0) {
            _controlBlock->_deleter(_controlBlock->_obj);
            delete _controlBlock; _controlBlock = nullptr;
        }
    }

    void reset(T* obj = nullptr, Deleter deleter = createDeleter())
    {
        clear();

        _controlBlock = createControlBlock(obj, std::move(deleter));
    }

    void swap(SharedPointer& rhs)
    {
        std::swap(_controlBlock, rhs._controlBlock);
    }

private:
    void increment()
    {
        if (use_count()) {
            ++_controlBlock->_counter;
        }
    }

    void decrement()
    {
        if (use_count()) {
            --_controlBlock->_counter;
        }
    }

    static utils::ControlBlock<T, Deleter>* createControlBlock(T* obj, Deleter deleter)
    {
        return new utils::ControlBlock<T, Deleter>{std::move(deleter), obj ? 1 : 0, obj};
    }

    static utils::DeleterFunc<T> createDeleter()
    {
        return [](T* toDelete){ delete toDelete; };
    }

private:
    utils::ControlBlock<T, Deleter>* _controlBlock{};
};

//Basic
//constructor
//destructor
//operator= assigns the weak_ptr

//Modifiers
//reset: releases the ownership of the managed object
//swap: swaps the managed objects

//Observers
//use_count: returns the number of shared_ptr objects that manage the object
//expired: checks whether the referenced object was already deleted
//lock: creates a shared_ptr that manages the referenced object
//owner_before: provides owner-based ordering of weak pointers

// Exists to track(non-own) an object, or to break cyclic references
// To use an object, pointed by the WeakPointer you need to cast it to the shared pointer.

//template<typename T>
//class WeakPointer
//{
//public:
//    WeakPointer(const WeakPointer<T>& rhs)
//    {}
//
//    WeakPointer(const SharedPointer<T>& rhs)
//    {}
//
//    ~WeakPointer()
//    {}
//
//private:
//    ControlBlock* _controlBlock{};
//};
