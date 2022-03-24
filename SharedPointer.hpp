#pragma once

// TODO: remove
#include <iostream>

#include <functional>
#include <atomic>

// TODO: add an allocator
// TODO: support weak pointers

// Modifiers:
// TODO: add reset
// TODO: add swap

// Observers:
// TODO: use_count
// TODO: unique
// TODO: operator[]

template<typename T,
         typename Deleter = std::function<void(T*)>>
class SharedPointer
{
public:
    explicit SharedPointer(T* obj) : SharedPointer(obj, [](T* t){ delete t; })
    {}

    SharedPointer(T* obj, Deleter deleter) : _controlBlock{new ControlBlock{std::move(deleter), 1, obj}}
    {}

    SharedPointer(const SharedPointer& rhs)
    { *this = rhs; }

    SharedPointer(SharedPointer&& rhs) noexcept
    { *this = std::move(rhs); }

    ~SharedPointer()
    { clear(); }

    SharedPointer& operator=(const SharedPointer& rhs)
    {
        if (this != &rhs) {
            clear();

            _controlBlock = rhs._controlBlock;
            ++_controlBlock->_counter;
        }

        return *this;
    }

    SharedPointer& operator=(SharedPointer&& rhs) noexcept
    {
        if (this != &rhs) {
            clear();

            _controlBlock = rhs._controlBlock;
            rhs._controlBlock = nullptr;
        }
        return *this;
    }

    T* get() const noexcept
    { return _controlBlock ? _controlBlock->_obj : nullptr; }

    T* operator->() const noexcept
    { return get(); }

    // TODO: fix this potential lost of common sense
    T& operator*() const noexcept
    { return *get(); }

    explicit operator bool() const noexcept
    { return _controlBlock && _controlBlock->_counter > 0; }

    void clear()
    {
        if (!_controlBlock) {
            return;
        }

        if (_controlBlock->_counter > 0) {
            --_controlBlock->_counter;
        }
        if (_controlBlock->_counter == 0) {
            _controlBlock->_deleter(_controlBlock->_obj);
            delete _controlBlock; _controlBlock = nullptr;
        }
    }

    // TODO: remove temporary methods
    void printFields() const
    {
        std::cout << "i: " << _controlBlock->_obj->i << std::endl;
        std::cout << "counter: " << _controlBlock->_counter << std::endl;
        std::cout << "sz: " << sizeof(*_controlBlock) << std::endl;
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
