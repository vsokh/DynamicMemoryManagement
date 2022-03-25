#pragma once

// TODO: remove
#include <iostream>

#include <functional>
#include <atomic>

// TODO: add an allocator
// TODO: support weak pointers

// Modifiers:
// TODO: add swap

// Observers:
// TODO: operator[]

template<typename T,
         typename Deleter = std::function<void(T*)>>
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
            _controlBlock = rhs._controlBlock;
            rhs._controlBlock = nullptr;
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

    // TODO: fix this potential lost of common sense
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

    void reset(T* obj, Deleter deleter = createDeleter())
    {
        clear();

        _controlBlock = createControlBlock(obj, std::move(deleter));
    }

    // TODO: remove temporary methods
    void printFields() const
    {
        std::cout << "i: " << _controlBlock->_obj->i << std::endl;
        std::cout << "counter: " << _controlBlock->_counter << std::endl;
        std::cout << "sz: " << sizeof(*_controlBlock) << std::endl;
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

    struct ControlBlock;
    static ControlBlock* createControlBlock(T* obj, Deleter deleter)
    {
        return new ControlBlock{std::move(deleter), obj ? 1 : 0, obj};
    }

    static std::function<void(T*)> createDeleter()
    {
        return [](T* toDelete){ delete toDelete; };
    }

private:
    struct ControlBlock
    {
        Deleter _deleter{};
        std::atomic<std::size_t> _counter{};
        T* _obj{};
    };

    ControlBlock* _controlBlock{};
};
