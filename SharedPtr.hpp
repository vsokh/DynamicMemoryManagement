#pragma once

#include "ControlBlock.hpp"

// TODO: support custom allocator
// TODO: support arrays, operator[]
// TODO: provide non-member functions
// TODO: ensure correctness in a multi-threaded environment

namespace base
{
    template<typename T>
    class WeakPtr;

    template<typename T, typename Deleter = details::DeleterFunc<T>>
    class SharedPtr {
    public:
        SharedPtr() = default;

        explicit SharedPtr(T *obj)
                : SharedPtr(obj, details::createDeleter<T, Deleter>())
        {
        }

        SharedPtr(T *obj, Deleter deleter)
                : _controlBlock{details::createControlBlock(obj, std::move(deleter))}
        {
        }

        SharedPtr(const SharedPtr &rhs)
                : _controlBlock{rhs._controlBlock}
        {
            details::increment(_controlBlock);
        }

        SharedPtr(SharedPtr &&rhs) noexcept
                : _controlBlock{rhs._controlBlock}
        {
            rhs._controlBlock = nullptr;
        }

        ~SharedPtr()
        {
            clear();
        }

        SharedPtr &operator=(const SharedPtr &rhs)
        {
            if (this != &rhs) {
                clear();

                _controlBlock = rhs._controlBlock;
                details::increment(_controlBlock);
            }

            return *this;
        }

        // TODO: rethink, test
        SharedPtr &operator=(SharedPtr &&rhs) noexcept
        {
            if (this != &rhs) {
                swap(rhs);
            }
            return *this;
        }

        T *operator->() const noexcept
        {
            return get();
        }

        T &operator*() const noexcept
        {
            return *get();
        }

        explicit operator bool() const noexcept
        {
            return use_count();
        }

        T *get() const noexcept
        {
            return _controlBlock ? _controlBlock->_obj : nullptr;
        }

        std::size_t use_count() const noexcept
        {
            return details::use_count(_controlBlock);
        }

        void reset(T *obj = nullptr, Deleter deleter = details::createDeleter<T, Deleter>())
        {
            clear();

            _controlBlock = createControlBlock(obj, std::move(deleter));
        }

        void swap(SharedPtr &rhs) noexcept
        {
            std::swap(_controlBlock, rhs._controlBlock);
        }

    private:
        void clear()
        {
            details::decrement(_controlBlock);
            details::releaseObj(_controlBlock);
            details::release(&_controlBlock);
        }

    private:
        template<typename Y>
        friend class WeakPtr;

        details::ControlBlock<T, Deleter> *_controlBlock{};
    };

} // namespace base