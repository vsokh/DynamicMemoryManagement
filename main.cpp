#include "SharedPointer.hpp"

#include <iostream>

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
    SharedPointer<Foo> p1(new Foo{100});
    SharedPointer<Foo> p2(new Foo{200});
    auto print = [&]() {
        std::cout << " p1=" << (p1 ? p1->print() : "nullptr");
        std::cout << " p2=" << (p2 ? p2->print() : "nullptr") << '\n';
    };
    print();

    p1.swap(p2);
    print();

    p1.reset();
    print();

    p1.swap(p2);
    print();
}
