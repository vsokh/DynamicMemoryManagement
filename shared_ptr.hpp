#pragma once

#include "control_block.hpp"

// TODO: support custom allocator
// TODO: support arrays, operator[]
// TODO: provide non-member functions
// TODO: ensure correctness in a multi-threaded environment

namespace base
{
    template<typename T>
    class weak_ptr;

    template<typename T>
    class shared_ptr {
    public:
        shared_ptr() = default;

        explicit shared_ptr(T *ptr)
                : _ptr{ptr}
                , _control_block{details::create_control_block(ptr)} {
        }

        template<typename Deleter>
        explicit shared_ptr(T *ptr, Deleter deleter)
                : _ptr{ptr}
                , _control_block{details::create_control_block(ptr, deleter)}
        {
        }
        shared_ptr(const shared_ptr& rhs)
                : _ptr{rhs._ptr}
                , _control_block{rhs._control_block} {
            details::increment(_control_block);
        }
        explicit shared_ptr(const weak_ptr<T>& rhs)
                : _ptr{rhs._ptr}
                , _control_block{rhs._control_block} {
            details::increment(_control_block);
        }
        shared_ptr(shared_ptr &&rhs) noexcept
                : _control_block{rhs._control_block} {
            rhs._control_block = nullptr;
        }

        ~shared_ptr() {
            clear();
        }

        shared_ptr &operator=(const shared_ptr &rhs) {
            shared_ptr(rhs).swap(*this);
            return *this;
        }
        shared_ptr &operator=(shared_ptr &&rhs) noexcept {
            swap(rhs);
            return *this;
        }

        T *operator->() const noexcept {
            return get();
        }
        T &operator*() const noexcept {
            return *get();
        }
        T *get() const noexcept {
            return _ptr;
        }

        explicit operator bool() const noexcept {
            return use_count();
        }

        std::size_t use_count() const noexcept {
            return details::use_count(_control_block);
        }

        void reset(T *ptr = nullptr) {
            clear();
            _ptr = ptr;
            _control_block = details::create_control_block(ptr);
        }
        template<typename Deleter>
        void reset(T *ptr, Deleter d) {
            clear();
            _ptr = ptr;
            _control_block = details::create_control_block(ptr, d);
        }

        void swap(shared_ptr &rhs) noexcept {
            std::swap(_control_block, rhs._control_block);
            std::swap(_ptr, rhs._ptr);
        }

        bool owner_before(const shared_ptr& rhs) {
            return _control_block < rhs._control_block;
        }
        bool owner_before(const weak_ptr<T>& rhs) {
            return _control_block < rhs._control_block;
        }

    private:
        void clear()
        {
            details::decrement(_control_block);
            details::releaseObj(_control_block);
            details::release(&_control_block);
        }

    private:
        template<typename Y>
        friend class weak_ptr;

        T* _ptr{};
        details::control_block* _control_block{};
    };

} // namespace base
