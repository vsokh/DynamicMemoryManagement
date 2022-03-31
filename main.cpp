#include "Memory.hpp"

#include <iostream>

// For testing
struct Foo {
//    explicit Foo(int _val) : val(_val) { std::cout << "Foo(" << val << ")...\n"; }
//    ~Foo() { std::cout << "~Foo(" << val << ")...\n"; }
    explicit Foo(int _val) : val(_val) {}
    int val;
};

// For testing
constexpr int testValue = 123;

void testSharedPtr()
{
    using namespace base;
    { //  default constructor
        SharedPtr<Foo> sp;

        std::cout << (0 == sp.use_count()) << std::endl;
    }
    { // copy-constructor
        SharedPtr sp1{new Foo{testValue}};
        SharedPtr sp2 = sp1;

        std::cout << (2 == sp1.use_count()) << std::endl;
    }
    { // copy-assignment-operator
        SharedPtr sp1{new Foo{testValue}};
        SharedPtr sp2{new Foo{testValue}};
        sp1 = sp2;

        std::cout << (2 == sp1.use_count()) << std::endl;
        std::cout << (2 == sp2.use_count()) << std::endl;
    }
    { // self copy-assignment
        SharedPtr sp{new Foo{testValue}};
        sp = sp;

        std::cout << (1 == sp.use_count()) << std::endl;
    }
    { // move-constructor
        SharedPtr sp1{new Foo{testValue}};
        SharedPtr sp2 = std::move(sp1);

        std::cout << (1 == sp2.use_count()) << std::endl;
    }
    { // move-assignment-operator
        SharedPtr sp1{new Foo{testValue}};
        SharedPtr sp2{new Foo{testValue}};
        sp1 = std::move(sp2);

        std::cout << (1 == sp1.use_count()) << std::endl;
    }
    { // self move-assignment
        SharedPtr sp{new Foo{testValue}};
        sp = std::move(sp);

        std::cout << (1 == sp.use_count()) << std::endl;
    }
    { // operator*
        SharedPtr sp{new Foo{testValue}};
        auto obj = *sp;
        std::cout << (obj.val == testValue) << std::endl;
    }
    { // operator->
        SharedPtr sp{new Foo{testValue}};
        std::cout << (sp->val == testValue) << std::endl;
    }
    { // operator bool
        SharedPtr sp1{new Foo{testValue}};
        std::cout << (sp1 ? 1 : 0) << std::endl;

        SharedPtr<Foo> sp2;
        std::cout << (!sp2 ? 1 : 0) << std::endl;
    }
    { // swap
        int val1 = 123;
        SharedPtr sp1{new Foo{val1}};

        int val2 = 321;
        SharedPtr sp2{new Foo{val2}};

        sp1.swap(sp2);
        std::cout << (sp2->val == val1 && sp1->val == val2) << std::endl;
        std::cout << (sp1.use_count() == 1 && sp2.use_count() == 1) << std::endl;
    }
    { // reset
        int val1 = 123;
        int val2 = 321;
        SharedPtr sp{new Foo{val1}};
        sp.reset(new Foo{val2});

        std::cout << (sp->val == val2) << std::endl;
    }
    { // get
        SharedPtr sp{new Foo{testValue}};
        auto obj = sp.get();
        std::cout << (obj->val == testValue) << std::endl;
    }
}

void testWeakPtr()
{
    using namespace base;

    { // default constructor
        WeakPtr<Foo> wp;

        std::cout << (0 == wp.use_count()) << std::endl;
    }
    { // copy-constructor with WeakPtr created from the same SharedPtr
        SharedPtr sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = wp1;

        std::cout << (1 == sp.use_count()) << std::endl;
        std::cout << (1 == wp1.use_count()) << std::endl;
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    { // copy-constructor with WeakPtrs created from different SharedPtrs
        SharedPtr sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr sp2{new Foo(testValue)};
        WeakPtr wp2 = wp1;

        std::cout << (1 == sp1.use_count()) << std::endl;
        std::cout << (1 == sp2.use_count()) << std::endl;
        std::cout << (1 == wp1.use_count()) << std::endl;
    }
    { // copy-assignment-operator with WeakPtrs created from different SharedPtrs
        SharedPtr sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr sp2{new Foo(testValue)};
        WeakPtr wp2 = sp2;

        wp2 = wp1;
        std::cout << (1 == sp1.use_count()) << std::endl;
        std::cout << (1 == sp2.use_count()) << std::endl;
        std::cout << (1 == wp1.use_count()) << std::endl;
    }
    { // self copy-assignment-operator
        SharedPtr sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = wp1;

        std::cout << (1 == sp.use_count()) << std::endl;
        std::cout << (1 == wp1.use_count()) << std::endl;
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    { // move-constructor with WeakPtrs created from the same SharedPtr
        SharedPtr sp{new Foo(testValue)};
        WeakPtr wp1 = sp;
        WeakPtr wp2 = std::move(wp1);

        std::cout << (1 == sp.use_count()) << std::endl;
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    { // self move-assignment
        SharedPtr sp{new Foo(testValue)};
        WeakPtr wp = sp;
        wp = std::move(wp);

        std::cout << (1 == wp.use_count()) << std::endl;
        std::cout << (1 == sp.use_count()) << std::endl;
    }
    { // move-assignment-operator with WeakPtrs created from different SharedPtrs
        SharedPtr sp1{new Foo(testValue)};
        WeakPtr wp1 = sp1;

        SharedPtr sp2{new Foo(testValue)};
        WeakPtr wp2 = sp2;

        wp2 = std::move(wp1);

        std::cout << (1 == sp1.use_count()) << std::endl;
        std::cout << (1 == sp2.use_count()) << std::endl;
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    { // reset
        SharedPtr sp{new Foo{123}}, sp1 = sp, sp2 = sp1;
        WeakPtr wp = sp;
        std::cout << (3 == sp.use_count()) << std::endl;
        std::cout << (3 == wp.use_count()) << std::endl;
        wp.reset();
        std::cout << (3 == sp.use_count()) << std::endl;
        std::cout << (0 == wp.use_count()) << std::endl;
    }
    { // lock
        SharedPtr sp{new Foo{123}};
        WeakPtr wp = sp;
        auto sp2 = wp.lock();
        std::cout << (2 == sp2.use_count()) << std::endl;
    }
    { // swap
        int val1 = 123;
        int val2 = 321;

        SharedPtr sp1{new Foo{val1}};
        SharedPtr sp2{new Foo{val2}};

        WeakPtr wp1 = sp1;
        WeakPtr wp2 = sp2;

        wp1.swap(wp2);
        auto sp = wp1.lock();
        std::cout << (val2 == sp->val) << std::endl;
    }
}

int main()
{
    testSharedPtr();
    testWeakPtr();
    return 0;
}
