#pragma once

#include <utility>

// TODO: capacity
// TODO: cbegin, cend
// TODO: element access

template<typename CharT = char>
class string {
public:
    using Char = CharT;
    using Size = std::size_t;

    explicit string(const Char *data)
            : _size{std::strlen(data)}, _data{new Char[_size + 1]} {
        std::strcpy(_data, data);
    }

    string(const string &rhs)
            : _size{rhs._size}, _data{new Char[_size + 1]} {
        std::strcpy(_data, rhs._data);
    }

    string(string &&rhs) noexcept
            : _size{rhs._size}, _data{rhs._data} {
        rhs._size = 0;
        rhs._data = nullptr;
    }

    ~string() {
        delete[] _data;
    }

    string &operator=(const string &rhs) {
        string{rhs}.swap(*this);
        return *this;
    }

    string &operator=(string &&rhs) noexcept {
        string{std::move(rhs)}.swap(*this);
        return *this;
    }

    void swap(string &rhs) noexcept {
        std::swap(_size, rhs._size);
        std::swap(_data, rhs._data);
    }

    bool empty() const noexcept
    {
        return _size == 0;
    }

    const Char* c_str() const noexcept
    {
        return _data;
    }

    Size size() const noexcept
    {
        return _size;
    }

    struct iterator {
        iterator() = default;

        explicit iterator(Char* data)
            : _data{data}
        {
        }

        bool operator!=(const iterator& rhs) const noexcept
        {
            return _data != rhs._data;
        }

        Char operator*() const noexcept
        {
            return *_data;
        }

        iterator& operator++()
        {
            ++_data;
            return *this;
        }

        iterator& operator--()
        {
            --_data;
            return *this;
        }

        void swap(iterator& rhs) noexcept
        {
            std::swap(_data, rhs._data);
        }

    private:
        Char* _data{};
    };

    iterator begin()
    {
        return iterator{_data};
    }

    iterator end()
    {
        return iterator{_data + _size};
    }

private:
    Size _size{};
    Char* _data{};
};
