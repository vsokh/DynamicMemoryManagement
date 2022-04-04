#pragma once

#include <functional>
#include <atomic>

namespace details
{
    using CounterType = std::size_t;
    struct ControlBlock
    {
        std::atomic<CounterType> _sharedCnt{};
        std::atomic<CounterType> _weakCnt{};

        ControlBlock(CounterType shared, CounterType weak)
            : _sharedCnt{shared}, _weakCnt{weak}
        {
        }

        virtual void onWeakZero() = 0;
        virtual ~ControlBlock() = default;
    };

    template<typename T, typename Deleter>
    struct ControlBlockPtr : public ControlBlock
    {
        T *_ptr{};
        Deleter _deleter{};

        ControlBlockPtr(T* ptr, Deleter deleter)
            : ControlBlock{1, 1}, _ptr{ptr}, _deleter{deleter}
        {
        }

        virtual void onWeakZero() override
        {
            if (_ptr) {
                _deleter(_ptr);
            }
        }
    };

    CounterType use_count(const ControlBlock* cb)
    {
        return cb ? cb->_sharedCnt.load() : 0;
    }

    CounterType weak_use_count(const ControlBlock* cb)
    {
        return cb ? cb->_weakCnt.load() : 0;
    }

    void incrementWeak(ControlBlock* cb)
    {
        if (weak_use_count(cb)) {
            ++cb->_weakCnt;
        }
    }
    void decrementWeak(ControlBlock* cb)
    {
        if (weak_use_count(cb)) {
            --cb->_weakCnt;
        }
    }

    void incrementShared(ControlBlock* cb)
    {
        if (use_count(cb)) {
            ++cb->_sharedCnt;
        }
    }
    void decrementShared(ControlBlock* cb)
    {
        if (use_count(cb)) {
            --cb->_sharedCnt;
        }
    }

    void increment(ControlBlock* cb)
    {
        incrementShared(cb);
        incrementWeak(cb);
    }
    void decrement(ControlBlock* cb)
    {
        decrementShared(cb);
        decrementWeak(cb);
    }

    void releaseObj(ControlBlock* cb)
    {
        if (cb && !use_count(cb)) {
            cb->onWeakZero();
        }
    }

    void release(ControlBlock** cb)
    {
        if (*cb && !weak_use_count(*cb)) {
            delete *cb; *cb = nullptr;
        }
    }

    template<typename Deleter, typename Allocator>
    decltype(auto) createControlBlock(std::nullptr_t, Deleter del, Allocator alloc)
    {
        auto ptr = alloc();
        const auto& ptrAmnt = ptr != nullptr;
        return new ControlBlockPtr<std::remove_pointer_t<decltype(ptr)>, Deleter>{del, ptrAmnt, ptrAmnt, ptr};
    }

    template<typename T, typename Deleter>
    decltype(auto) createControlBlock(T* ptr, Deleter del)
    {
        const auto& ptrAmnt = ptr != nullptr;
        return new ControlBlockPtr<T, Deleter>{del, ptrAmnt, ptrAmnt, ptr};
    }

    template<typename T>
    void deleter(T* toDelete)
    {
        delete toDelete;
    }

} // namespace details
