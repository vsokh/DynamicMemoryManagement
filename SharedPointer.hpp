#pragma once

// TODO: remove
#include <iostream>
#include <functional>
#include <atomic>

template<typename T, typename Deleter = std::function<void(T*)>>
class SharedPointer
{
    using this_type = SharedPointer<T, Deleter>;
    using const_this_type = const this_type;
    using const_this_type_ptr = const_this_type*;
    using const_this_type_ref = const_this_type&;

public:
    explicit SharedPointer(T* obj) : SharedPointer(obj, [](T* t){ delete t; })
    {}

    SharedPointer(T* obj, Deleter deleter) : _controlBlock{new ControlBlock{std::move(deleter), 1, obj}}
    {}

    SharedPointer(const SharedPointer<T, Deleter>& rhs)
    { *this = rhs; }

    SharedPointer(SharedPointer<T, Deleter>&& rhs) noexcept
    { *this = std::move(rhs); }

    ~SharedPointer()
    { clear(); }

    const T* operator->() const
    { return _controlBlock ? _controlBlock->_obj : nullptr; }

    const T& operator*() const
    { return *_controlBlock->_obj; }

    T* operator->()
    { return const_cast<const_this_type_ptr>(this)->_controlBlock->_obj; }

    T& operator*()
    { return *const_cast<const_this_type_ptr>(this)->_controlBlock->_obj; }

    SharedPointer<T, Deleter>& operator=(const SharedPointer<T, Deleter>& rhs)
    {
        if (this != &rhs) {
            clear();

            _controlBlock = rhs._controlBlock;
            ++_controlBlock->_counter;
        }

        return *this;
    }

    SharedPointer<T, Deleter>& operator=(SharedPointer<T, Deleter>&& rhs) noexcept
    {
        if (this != &rhs) {
            clear();

            _controlBlock = rhs._controlBlock;
            rhs._controlBlock = nullptr;
        }
        return *this;
    }

    void clear()
    {
        if (!_controlBlock) {
            return;
        }

        --_controlBlock->_counter;
        if (_controlBlock->_counter == 0) {
            _controlBlock->deleter(_controlBlock->_obj);
            delete _controlBlock; _controlBlock = nullptr;
        }
    }

    // TODO: remove temporary methods
    void printFields() const
    {
        std::cout << "obj: " << _controlBlock->_obj->i << std::endl;
        std::cout << "counter: " << _controlBlock->_counter << std::endl;
        std::cout << "sz: " << sizeof(*_controlBlock) << std::endl;
    }

private:
    struct ControlBlock
    {
        Deleter deleter{};
        std::atomic<std::size_t> _counter{};
        T* _obj{};
    };

    ControlBlock* _controlBlock{};
};
