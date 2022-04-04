#pragma once

#include <functional>
#include <atomic>

namespace details
{
    using counter_type = std::size_t;
    struct control_block
    {
        std::atomic<counter_type> _shared_cnt{};
        std::atomic<counter_type> _weak_cnt{};

        control_block(counter_type shared, counter_type weak)
                : _shared_cnt{shared}
                , _weak_cnt{weak}
        {
        }

        virtual void on_weak_zero() = 0;
        virtual ~control_block() = default;
    };

    template<typename T, typename Deleter>
    struct control_block_ptr : public control_block
    {
        T *_ptr{};
        Deleter _deleter{};

        control_block_ptr(T* ptr, Deleter deleter)
                : control_block{ptr!=nullptr, ptr!=nullptr}
                , _ptr{ptr}
                , _deleter{deleter}
        {
        }

        void on_weak_zero() override {
            if (_ptr) {
                _deleter(_ptr);
            }
        }
    };

    counter_type use_count(const control_block* cb) {
        return cb ? cb->_shared_cnt.load() : 0;
    }

    counter_type weak_use_count(const control_block* cb) {
        return cb ? cb->_weak_cnt.load() : 0;
    }

    void increment_weak(control_block* cb) {
        if (weak_use_count(cb)) {
            ++cb->_weak_cnt;
        }
    }
    void decrement_weak(control_block* cb) {
        if (weak_use_count(cb)) {
            --cb->_weak_cnt;
        }
    }

    void increment_shared(control_block* cb) {
        if (use_count(cb)) {
            ++cb->_shared_cnt;
        }
    }
    void decrement_shared(control_block* cb) {
        if (use_count(cb)) {
            --cb->_shared_cnt;
        }
    }

    void increment(control_block* cb) {
        increment_shared(cb);
        increment_weak(cb);
    }
    void decrement(control_block* cb) {
        decrement_shared(cb);
        decrement_weak(cb);
    }

    void releaseObj(control_block* cb) {
        if (cb && !use_count(cb)) {
            cb->on_weak_zero();
        }
    }

    void release(control_block** cb) {
        if (*cb && !weak_use_count(*cb)) {
            delete *cb; *cb = nullptr;
        }
    }

    template<typename T>
    decltype(auto) create_control_block(T* ptr) {
        auto del = [](T* del){delete del;};
        return new control_block_ptr<T, decltype(del)>{ptr, del};
    }

    template<typename T, typename Deleter>
    decltype(auto) create_control_block(T* ptr, Deleter del) {
        return new control_block_ptr<T, Deleter>{ptr, del};
    }
} // namespace details
