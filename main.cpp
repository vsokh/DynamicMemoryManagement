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
    const auto& sp2 = sp1;
    int i = sp2->i;
//    sp2->i = 4;

    auto& sp3 = sp1;
    sp3->i = 4;
    return 0;
}