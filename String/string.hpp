#pragma once

#include <utility>
#include <iostream>

// TODO: cbegin, cend
// TODO: element access

template<typename CharT>
class basic_string {
public:
    using Size = std::size_t;
    static const Size buffer = sizeof(void*);

    explicit basic_string(const CharT *data)
            : _size{std::strlen(data)}
            , _capacity{std::max(sizeof(basic_string), _size)}
            , _data{new CharT[_capacity]}
    {
        std::strcpy(_data, data);
    }

    basic_string(const basic_string &rhs)
            : _size{rhs._size}
            , _capacity{rhs._capacity}
            , _data{new CharT[_capacity]}
    {
        std::strcpy(_data, rhs._data);
    }

    basic_string(basic_string &&rhs) noexcept
            : _size{rhs._size}
            , _capacity{rhs._capacity}
            , _data{rhs._data}
    {
        rhs._size = 0;
        rhs._data = nullptr;
    }

    ~basic_string()
    {
        delete[] _data;
    }

    basic_string &operator=(const basic_string &rhs)
    {
        basic_string{rhs}.swap(*this);
        return *this;
    }

    basic_string &operator=(basic_string &&rhs) noexcept
    {
        basic_string{std::move(rhs)}.swap(*this);
        return *this;
    }

    void append(const CharT* data, Size pos, Size count)
    {
        auto oldSize = _size;
        if (auto newSize = oldSize + count;
                newSize >= _capacity) {
            auto newCapacity = newSize + buffer;
            auto tmpData = new CharT[newCapacity];
            std::strcpy(tmpData, _data);

            delete[] _data;
            _data = tmpData;
            _size = newSize;
            _capacity = newCapacity;
        }
        for (auto idx = 0; idx < count; ++idx) {
            _data[oldSize + idx] = data[pos + idx];
        }
    }

    basic_string operator+(const CharT* data)
    {
        basic_string tmp{*this};
        tmp.append(data, 0, std::strlen(data));
        return tmp;
    }
    basic_string operator+(const basic_string& rhs)
    {
        basic_string tmp{*this};
        tmp.append(rhs._data, 0, rhs._size);
        return tmp;
    }

    basic_string& operator+=(const CharT* data)
    {
        append(data, 0, std::strlen(data));
        return *this;
    }
    basic_string& operator+=(const basic_string& rhs)
    {
        append(rhs._data, 0, rhs._size);
        return *this;
    }
    void swap(basic_string &rhs) noexcept
    {
        std::swap(_size, rhs._size);
        std::swap(_capacity, rhs._capacity);
        std::swap(_data, rhs._data);
    }

    bool empty() const noexcept
    {
        return _size == 0;
    }

    const CharT* c_str() const noexcept
    {
        return _data;
    }

    Size size() const noexcept
    {
        return _size;
    }

    Size capacity() const noexcept
    {
        return _capacity;
    }

    struct iterator {
        iterator() = default;

        explicit iterator(CharT* data)
                : _data{data}
        {
        }

        bool operator!=(const iterator& rhs) const noexcept
        {
            return _data != rhs._data;
        }

        CharT operator*() const noexcept
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
        CharT* _data{};
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
    Size _size;
    Size _capacity;
    CharT* _data;
};

using string = basic_string<char>;
