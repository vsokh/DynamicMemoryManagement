#pragma once

#include "ControlBlock.hpp"

// Description: Exists to track(not own) an object, or to break cyclic references.
//              To use an object, pointed by WeakPtr, you need to cast it to the shared pointer.

// API:
// Modifiers:
// reset: releases the ownership of the managed object
// swap: swaps the managed objects

// Observers:
// use_count: returns the number of SharedPtr objects that manage the object
// expired: checks whether the referenced object was already deleted
// lock: creates a SharedPtr that manages the referenced object
// owner_before: provides owner-based ordering of weak pointers

namespace base {

    template<typename T, typename Deleter>
    class SharedPtr;

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

        std::size_t use_count() const noexcept
        {
            return details::weak_use_count(_controlBlock);
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
            clear();

            _controlBlock = ptr._controlBlock;
            details::incrementWeak(_controlBlock);
        }

    private:
        details::ControlBlock<T> *_controlBlock{};
    };
} // namespace base