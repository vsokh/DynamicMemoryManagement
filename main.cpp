#include "Memory.hpp"

#include <iostream>
#include <memory>

// For testing
struct Foo {
    Foo(int _val) : val(_val) { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
    std::string print() { return std::to_string(val); }
    int val;
};

int main()
{
    using namespace mem;

    SharedPtr<Foo> sp1{new Foo(123)};
    WeakPtr<Foo> wp1 = sp1;

    SharedPtr<Foo> sp2{new Foo(123)};
    WeakPtr<Foo> wp2 = sp2;

    wp1 = std::move(wp2);

    return 0;
}
