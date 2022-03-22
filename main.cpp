#include <iostream>

template<typename T>
class SharedPointer
{
public:
    SharedPointer(T* obj)
        : _cb{new ControlBlock{obj, new size_t{1}}}
    {
    }

    ~SharedPointer() {
        if (_cb->_counter == 0) {
            delete _cb->_obj;
            delete _cb;
        } else {
            --*_cb->_counter;
        }
    }

    SharedPointer(const SharedPointer<T>& rhs)
        : _cb{rhs._cb}
    {
        // TODO: hangle nullptr cb
        *rhs._cb->_counter += 1;
    }

    T* operator->() const
    {
        if (_cb == nullptr)
        {
            std::cout << "obj is nullptr" << std::endl;
            return nullptr;
        }
        return _cb->_obj;
    }

    T& operator*() const
    {
        // TODO: handle nullptr obj
        return *_cb->_obj;
    }

    void printFields()
    {
        std::cout << "obj: " << _cb->_obj->i << std::endl;
        std::cout << "counter: " << *_cb->_counter << std::endl;
    }

private:
    struct ControlBlock
    {
        T* _obj{};
        size_t* _counter{};
    } *_cb{};
};

struct S
{
    int i{5};
};

template<typename T>
struct TD;

int main()
{

    SharedPointer<S> sp1{new S};
    SharedPointer<S> sp2{sp1};
    {
        SharedPointer<S> sp3{sp2};
        sp3.printFields();
    }
    sp1.printFields();
    sp2.printFields();
    return 0;
}
