#include "SharedPointer.hpp"

// For testing
struct S
{
    int i{5};
};

// For type checking
template<typename T>
struct TD;

int main()
{
    SharedPointer<S> sp1{new S, [](S *s){
        std::cout << "log: " << s->i << std::endl;
        delete s;
    }};
    sp1.printFields();
    return 0;
}