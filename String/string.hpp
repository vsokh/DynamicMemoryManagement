#pragma once

#include <utility>
#include <iostream>

// TODO: cbegin, cend
// TODO: element access

namespace utils {
    template<typename CharT, typename Size>
    void ncpy(CharT* dst, const CharT* src, Size count)
    {
        std::strncpy(dst, src, count);
    }

    template<typename CharT>
    auto len(const CharT* str)
    {
        return std::strlen(str);
    }
}

template<typename CharT>
class basic_string {
public:
    using Size = std::size_t;
    static const Size buffer = sizeof(void*);

    explicit basic_string(const CharT *data)
            : _size{utils::len(data)}
            , _capacity{std::max(sizeof(basic_string), _size)}
            , _data{new CharT[_capacity]}
    {
        utils::ncpy(_data, data, _size);
    }

    explicit basic_string(CharT data)
            : _size{1}
            , _capacity{sizeof(basic_string)}
            , _data{new CharT[_capacity]}
    {
        utils::ncpy(_data, &data, _size);
    }

    basic_string(const basic_string &rhs)
            : _size{rhs._size}
            , _capacity{rhs._capacity}
            , _data{new CharT[_capacity]}
    {
        utils::ncpy(_data, rhs._data, _size);
    }

    basic_string(basic_string &&rhs) noexcept
            : _size{rhs._size}
            , _capacity{rhs._capacity}
            , _data{rhs._data}
    {
        rhs._size = 0;
        rhs._capacity = 0;
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
        auto newSize = _size + count;
        if (newSize >= _capacity) {
            auto newCapacity = newSize + buffer;
            auto tmpData = new CharT[newCapacity];
            utils::ncpy(tmpData, _data, _size);

            delete[] _data;
            _data = tmpData;
            _capacity = newCapacity;
        }
        for (auto idx = 0; idx < count; ++idx) {
            _data[_size + idx] = data[pos + idx];
        }
        _size = newSize;
    }

    basic_string& operator+=(CharT rhs)
    {
        append(&rhs, 0, 1);
        return *this;
    }
    basic_string& operator+=(const CharT* rhs)
    {
        append(rhs, 0, utils::len(rhs));
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

    [[nodiscard]] bool empty() const noexcept
    {
        return _size == 0;
    }

    const CharT* c_str() const noexcept
    {
        return _data;
    }

    [[nodiscard]] Size size() const noexcept
    {
        return _size;
    }

    [[nodiscard]] Size capacity() const noexcept
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
    Size _size{};
    Size _capacity{};
    CharT* _data;
};

template<typename CharT>
basic_string<CharT> operator+(const basic_string<CharT>& lhs,
                              const basic_string<CharT>& rhs)
{
    basic_string tmp{lhs};
    tmp.append(rhs.c_str(), 0, rhs.size());
    return tmp;
}

template<typename CharT>
basic_string<CharT> operator+(const basic_string<CharT>& lhs,
                              const CharT* rhs)
{
    return lhs + basic_string{rhs};
}

template<typename CharT>
basic_string<CharT> operator+(const basic_string<CharT>& lhs,
                              const CharT rhs)
{
    return lhs + basic_string{rhs};
}

using string = basic_string<char>;
