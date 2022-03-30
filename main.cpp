#include "Memory.hpp"

#include <iostream>

// For testing
struct Foo {
    explicit Foo(int _val) : val(_val) { std::cout << "Foo(" << val << ")...\n"; }
    ~Foo() { std::cout << "~Foo(" << val << ")...\n"; }
    int val;
};

// For testing
constexpr int testValue = 123;

void testSharedPtr()
{
}

void testWeakPtr()
{
    using namespace base;

    { // default constructor
        WeakPtr<Foo> wp;
    }
    { // copy-constructor with WeakPtr created from the same SharedPtr
        SharedPtr<Foo> sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = wp1;
    }
    { // copy-constructor with WeakPtrs created from a different SharedPtr
        SharedPtr<Foo> sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr<Foo> sp2{new Foo(testValue)};
        WeakPtr wp2 = wp1;
    }
    { // copy-assignment-operator with WeakPtrs created from a different SharedPtr
        SharedPtr<Foo> sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr<Foo> sp2{new Foo(testValue)};
        WeakPtr wp2 = sp2;

        wp2 = wp1;
    }
    { // self copy-assignment-operator
        SharedPtr<Foo> sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = wp1;
    }
    { // move-constructor with WeakPtrs created from the same SharedPtr
        SharedPtr<Foo> sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = std::move(wp1);
    }
    { // self move-assignment
        SharedPtr<Foo> sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        wp1 = std::move(wp1);
    }
    { // move-assignment-operator with WeakPtrs created from different SharedPtrs
        SharedPtr<Foo> sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr<Foo> sp2{new Foo(testValue)};
        WeakPtr wp2 = sp2;

        wp2 = std::move(wp1);
    }
}

int main()
{
    testSharedPtr();
    testWeakPtr();
    return 0;
}
