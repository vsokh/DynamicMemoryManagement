#include "SharedPointer.hpp"

#include <iostream>
#include <memory>

// For testing
struct Foo {
    Foo(int _val) : val(_val) { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
    std::string print() { return std::to_string(val); }
    int val;
};

// For type checking
template<typename T>
struct TD;

int main()
{
    SharedPointer<Foo> sp(new Foo(123));
    auto sp1 = sp;
    return 0;
}
