#pragma once

#include "ControlBlock.hpp"

// Observers:
// TODO: owner_before: provides owner-based ordering of weak pointers

// TODO: ensure correctness in a multi-threaded environment

namespace base
{
    template<typename T, typename Deleter>
    class SharedPtr;

    // Description: Exists to track(not own) an object, or to break cyclic references.
    //              To use an object, pointed by WeakPtr, you need to cast it to the shared pointer.
    template<typename T>
    class WeakPtr {
    public:
        WeakPtr() = default;

        WeakPtr(const WeakPtr &rhs)
                : _controlBlock{rhs._controlBlock}
        {
            details::incrementWeak(_controlBlock);
        }

        WeakPtr(const SharedPtr<T> &rhs)
                : _controlBlock{rhs._controlBlock}
        {
            details::incrementWeak(_controlBlock);
        }

        WeakPtr(WeakPtr &&rhs)
                : _controlBlock{rhs._controlBlock}
        {
            rhs._controlBlock = nullptr;
        }

        WeakPtr &operator=(const WeakPtr &rhs)
        {
            if (this != &rhs) {
                copy(rhs);
            }
            return *this;
        }

        WeakPtr &operator=(const SharedPtr<T> &rhs)
        {
            clear();
            copy(rhs);
            return *this;
        }

        WeakPtr &operator=(WeakPtr &&rhs) noexcept
        {
            if (this != &rhs) {
                clear();

                _controlBlock = rhs._controlBlock;
                rhs._controlBlock = nullptr;
            }
            return *this;
        }

        ~WeakPtr()
        {
            clear();
        }

        bool expired() const noexcept
        {
            return use_count() == 0;
        }

        SharedPtr<T> lock() const noexcept
        {
            return expired() ? SharedPtr<T>{} : SharedPtr<T>{*this};
        }

        void reset() noexcept
        {
            clear();
            _controlBlock = nullptr;
        }

        void swap(WeakPtr& rhs) noexcept
        {
            std::swap(_controlBlock, rhs._controlBlock);
        }

        std::size_t use_count() const noexcept
        {
            return details::use_count(_controlBlock);
        }

    private:
        void clear()
        {
            details::decrementWeak(_controlBlock);
            details::release(&_controlBlock);
        }

        template<typename SmartPointer>
        void copy(const SmartPointer& ptr)
        {
            _controlBlock = ptr._controlBlock;
            details::incrementWeak(_controlBlock);
        }

    private:
        template<typename Y, typename Deleter>
        friend class SharedPtr;

        details::ControlBlock<T> *_controlBlock{};
    };
} // namespace base