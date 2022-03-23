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
// TODO: add get
// TODO: operator[]
// TODO: operator bool
// TODO: use_count
// TODO: unique

template<typename T, typename Deleter = std::function<void(T*)>>
class SharedPointer
{
public:
    explicit SharedPointer(T* obj) : SharedPointer(obj, [](T* t){ delete t; })
    {}

    SharedPointer(T* obj, Deleter deleter) : _cb{new ControlBlock{std::move(deleter), 1, obj}}
    {}

    SharedPointer(const SharedPointer<T, Deleter>& rhs)
    { *this = rhs; }

    SharedPointer(SharedPointer<T, Deleter>&& rhs) noexcept
    { *this = std::move(rhs); }

    ~SharedPointer()
    { clear(); }

    T* operator->() const
    { return _cb ? _cb->_obj : nullptr; }

    T& operator*() const
    {
        // TODO: handle nullptr obj
        return *_cb->_obj;
    }

    SharedPointer<T, Deleter>& operator=(const SharedPointer<T, Deleter>& rhs)
    {
        if (this != &rhs) {
            clear();

            _cb = rhs._cb;
            ++_cb->_counter;
        }

        return *this;
    }

    SharedPointer<T, Deleter>& operator=(SharedPointer<T, Deleter>&& rhs) noexcept
    {
        if (this != &rhs) {
            clear();

            _cb = rhs._cb;
            rhs._cb = nullptr;
        }
        return *this;
    }

    void clear()
    {
        if (!_cb) {
            return;
        }

        --_cb->_counter;
        if (_cb->_counter == 0) {
            _cb->deleter(_cb->_obj);
            delete _cb; _cb = nullptr;
        }
    }

    // TODO: remove temporary methods
    void printFields()
    {
        std::cout << "obj: " << _cb->_obj->i << std::endl;
        std::cout << "counter: " << _cb->_counter << std::endl;
        std::cout << "sz: " << sizeof(*_cb) << std::endl;
    }

private:
    struct ControlBlock
    {
        Deleter deleter{};
        std::atomic<std::size_t> _counter{};
        T* _obj{};
    };

    ControlBlock* _cb{};
};
