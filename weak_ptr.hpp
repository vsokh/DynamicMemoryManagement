#pragma once

#include "control_block.hpp"

// TODO: ensure correctness in a multi-threaded environment

namespace base
{
    template<typename T>
    class shared_ptr;

    // Description: Exists to track(not own) an object, or to break cyclic references.
    //              To use an object, pointed by weak_ptr, you need to cast it to the shared pointer.
    template<typename T>
    class weak_ptr
    {
    public:
        weak_ptr() = default;

        weak_ptr(const weak_ptr &rhs)
                : _ptr{rhs._ptr}
                , _control_block{rhs._control_block}
        {
            details::increment_weak(_control_block);
        }

        weak_ptr(const shared_ptr<T> &rhs)
                : _ptr{rhs._ptr}
                , _control_block{rhs._control_block}
        {
            details::increment_weak(_control_block);
        }

        weak_ptr(weak_ptr &&rhs) noexcept
                : _ptr{rhs._ptr}
                , _control_block{rhs._control_block}
        {
            rhs._ptr = nullptr;
            rhs._control_block = nullptr;
        }

        weak_ptr &operator=(const weak_ptr &rhs)
        {
            weak_ptr(rhs).swap(*this);
            return *this;
        }

        weak_ptr &operator=(const shared_ptr<T> &rhs)
        {
            shared_ptr(rhs).swap(*this);
            return *this;
        }

        weak_ptr &operator=(weak_ptr &&rhs) noexcept
        {
            weak_ptr(std::move(rhs)).swap(*this);
            return *this;
        }

        ~weak_ptr()
        {
            details::decrement_weak(_control_block);
            details::release(&_control_block);
        }

        bool expired() const noexcept
        {
            return use_count() == 0;
        }

        shared_ptr<T> lock() const noexcept
        {
            return expired() ? shared_ptr<T>{} : shared_ptr<T>{*this};
        }

        void reset() noexcept
        {
            weak_ptr{}.swap(*this);
        }

        void swap(weak_ptr& rhs) noexcept
        {
            std::swap(_ptr, rhs._ptr);
            std::swap(_control_block, rhs._control_block);
        }

        std::size_t use_count() const noexcept
        {
            return details::use_count(_control_block);
        }

        bool owner_before(const shared_ptr<T>& rhs)
        {
            return _control_block < rhs._control_block;
        }

        bool owner_before(const weak_ptr<T>& rhs)
        {
            return _control_block < rhs._control_block;
        }

    private:
        template<typename Y>
        friend class shared_ptr;

        T* _ptr{};
        details::control_block* _control_block{};
    };
} // namespace base