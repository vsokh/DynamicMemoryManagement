#pragma once

#include <functional>
#include <atomic>

namespace details
{

    template<typename T>
    using DeleterFunc = std::function<void(T*)>;
    using CounterType = std::size_t;

    template<typename T, typename Deleter = DeleterFunc<T>>
    struct ControlBlock
    {
        Deleter _deleter{};
        std::atomic<CounterType> _sharedCounter{};
        std::atomic<CounterType> _weakCounter{};
        T *_obj{};
    };

    template<typename T, typename Deleter = DeleterFunc<T>>
    CounterType use_count(const ControlBlock<T, Deleter>* controlBlock)
    {
        return controlBlock ? controlBlock->_sharedCounter.load() : 0;
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    CounterType weak_use_count(const ControlBlock<T, Deleter>* controlBlock)
    {
        return controlBlock ? controlBlock->_weakCounter.load() : 0;
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void incrementWeak(ControlBlock<T, Deleter>* controlBlock)
    {
        if (weak_use_count(controlBlock)) {
            ++controlBlock->_weakCounter;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void decrementWeak(ControlBlock<T, Deleter>* controlBlock)
    {
        if (weak_use_count(controlBlock)) {
            --controlBlock->_weakCounter;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void incrementShared(ControlBlock<T, Deleter>* controlBlock)
    {
        if (use_count(controlBlock)) {
            ++controlBlock->_sharedCounter;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void decrementShared(ControlBlock<T, Deleter>* controlBlock)
    {
        if (use_count(controlBlock)) {
            --controlBlock->_sharedCounter;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void increment(ControlBlock<T, Deleter>* controlBlock)
    {
        incrementShared(controlBlock);
        incrementWeak(controlBlock);
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void decrement(ControlBlock<T, Deleter>* controlBlock)
    {
        decrementShared(controlBlock);
        decrementWeak(controlBlock);
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void releaseObj(ControlBlock<T, Deleter>* controlBlock)
    {
        if (controlBlock && !use_count(controlBlock)) {
            delete controlBlock->_obj; controlBlock->_obj = nullptr;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    void release(ControlBlock<T, Deleter>** controlBlock)
    {
        if (*controlBlock && !weak_use_count(*controlBlock)) {
            delete *controlBlock; *controlBlock = nullptr;
        }
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    details::ControlBlock<T, Deleter>* createControlBlock(T* obj, Deleter deleter)
    {
        const auto& exist = obj != nullptr;
        return new ControlBlock<T, Deleter>{std::move(deleter), exist, exist, obj};
    }

    template<typename T, typename Deleter = DeleterFunc<T>>
    DeleterFunc<T> createDeleter()
    {
        return [](T* toDelete){ delete toDelete; };
    }

} // namespace details
