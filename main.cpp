#include "mem.hpp"
#include <iostream>

// For testing
constexpr int testValue = 123;
struct Foo {
//    explicit Foo(int _val) : val(_val) { std::cout << "Foo(" << val << ")...\n"; }
//    ~Foo() { std::cout << "~Foo(" << val << ")...\n"; }
    explicit Foo(int _val) : val(_val) {}
    int val;
};

void test_shared_ptr()
{
    using namespace base;
    std::cout << "shared_ptr" << std::endl;
    {
        std::cout << "default constructor: ";
        shared_ptr<Foo> sp;

        std::cout << (0 == sp.use_count()) << std::endl;
    }
    {
        std::cout << "copy-constructor: ";
        shared_ptr sp1{new Foo{testValue}};
        shared_ptr sp2 = sp1;

        std::cout << (2 == sp1.use_count()) << std::endl;
    }
    {
        std::cout << "copy-assignment-operator: ";
        shared_ptr sp1{new Foo{testValue}};
        shared_ptr sp2{new Foo{testValue}};
        sp1 = sp2;

        std::cout << (2 == sp1.use_count()) << ", ";
        std::cout << (2 == sp2.use_count()) << ", ";

        shared_ptr sp3{new Foo{testValue}};
        shared_ptr sp4 = sp3;
        std::cout << (2 == sp4.use_count()) << ", ";
        sp4 = std::move(sp3);
        std::cout << (1 == sp4.use_count()) << std::endl;
    }
    {
        std::cout << "self copy-assignment: ";
        shared_ptr sp{new Foo{testValue}};
        sp = sp;

        std::cout << (1 == sp.use_count()) << std::endl;
    }
    {
        std::cout << "move-constructor: ";
        shared_ptr sp1{new Foo{testValue}};
        shared_ptr sp2 = std::move(sp1);

        std::cout << (1 == sp2.use_count()) << std::endl;
    }
    {
        std::cout << "move-assignment-operator: ";
        shared_ptr sp1{new Foo{testValue}};
        shared_ptr sp2{new Foo{testValue}};
        sp1 = std::move(sp2);

        std::cout << (1 == sp1.use_count()) << std::endl;
    }
    {
        std::cout << "self move-assignment: ";
        shared_ptr sp{new Foo{testValue}};
        sp = std::move(sp);

        std::cout << (1 == sp.use_count()) << std::endl;
    }
    {
        std::cout << "operator*: ";
        shared_ptr sp{new Foo{testValue}};
        auto obj = *sp;
        std::cout << (obj.val == testValue) << std::endl;
    }
    {
        std::cout << "operator->: ";
        shared_ptr sp{new Foo{testValue}};
        std::cout << (sp->val == testValue) << std::endl;
    }
    {
        std::cout << "operator bool: ";
        shared_ptr sp1{new Foo{testValue}};
        std::cout << (sp1 ? 1 : 0) << ", ";

        shared_ptr<Foo> sp2;
        std::cout << (!sp2 ? 1 : 0) << std::endl;
    }
    {
        std::cout << "swap: ";
        int val1 = 123;
        shared_ptr sp1{new Foo{val1}};

        int val2 = 321;
        shared_ptr sp2{new Foo{val2}};

        sp1.swap(sp2);
        std::cout << (sp2->val == val1 && sp1->val == val2) << ", ";
        std::cout << (sp1.use_count() == 1 && sp2.use_count() == 1) << std::endl;
    }
    {
        std::cout << "reset: ";
        int val1 = 123;
        int val2 = 321;
        shared_ptr sp{new Foo{val1}};
        sp.reset(new Foo{val2});

        std::cout << (sp->val == val2) << std::endl;
    }
    {
        std::cout << "get: ";
        shared_ptr sp{new Foo{testValue}};
        auto obj = sp.get();
        std::cout << (obj->val == testValue) << std::endl;
    }
    {
        std::cout << "owner_before: ";
        auto sp1 = shared_ptr<Foo>{new Foo{123}};
        auto sp2 = shared_ptr<Foo>{new Foo{123}};
        std::cout << !sp1.owner_before(sp1) << ", ";
        std::cout << sp1.owner_before(sp2) << ", ";
        std::cout << !sp2.owner_before(sp1) << std::endl;
    }
    {
        std::cout << "allocator: ";
        int val = 1;
        shared_ptr<int> sp{nullptr,
                                 [&val](int* i){std::cout << (*i == val) << std::endl; delete i;},
                                 [&val](){return new int(val);}};
    }
    std::cout << std::endl;
}

void test_weak_ptr()
{
    using namespace base;
    std::cout << "weak_ptr" << std::endl;
    {
        std::cout << "default constructor: ";
        weak_ptr<Foo> wp;

        std::cout << (0 == wp.use_count()) << std::endl;
    }
    {
        std::cout << "copy-constructor with weak_ptr created from the same shared_ptr: ";
        shared_ptr sp{new Foo(testValue)};
        weak_ptr wp1 = sp;
        weak_ptr wp2 = wp1;

        std::cout << (1 == sp.use_count()) << ", ";
        std::cout << (1 == wp1.use_count()) << ", ";
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    {
        std::cout << "copy-constructor with weak_ptrs created from different shared_ptrs: ";
        shared_ptr sp1{new Foo(testValue)};
        weak_ptr wp1 = sp1;

        shared_ptr sp2{new Foo(testValue)};
        weak_ptr wp2 = wp1;

        std::cout << (1 == sp1.use_count()) << ", ";
        std::cout << (1 == sp2.use_count()) << ", ";
        std::cout << (1 == wp1.use_count()) << std::endl;
    }
    {
        std::cout << "copy-assignment-operator with weak_ptrs created from different shared_ptrs: ";
        shared_ptr sp1{new Foo(testValue)};
        weak_ptr wp1 = sp1;

        shared_ptr sp2{new Foo(testValue)};
        weak_ptr wp2 = sp2;

        wp2 = wp1;
        std::cout << (1 == sp1.use_count()) << ", ";
        std::cout << (1 == sp2.use_count()) << ", ";
        std::cout << (1 == wp1.use_count()) << std::endl;
    }
    {
        std::cout << "self copy-assignment-operator: ";
        shared_ptr sp{new Foo(testValue)};
        weak_ptr wp1 = sp;
        weak_ptr wp2 = wp1;

        std::cout << (1 == sp.use_count()) << ", ";
        std::cout << (1 == wp1.use_count()) << ", ";
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    {
        std::cout << "move-constructor with weak_ptrs created from the same shared_ptr: ";
        shared_ptr sp{new Foo(testValue)};
        weak_ptr wp1 = sp;
        weak_ptr wp2 = std::move(wp1);

        std::cout << (1 == sp.use_count()) << ", ";
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    {
        std::cout << "self move-assignment: ";
        shared_ptr sp{new Foo(testValue)};
        weak_ptr wp = sp;
        wp = std::move(wp);

        std::cout << (1 == wp.use_count()) << ", ";
        std::cout << (1 == sp.use_count()) << std::endl;
    }
    {
        std::cout << "move-assignment-operator with weak_ptrs created from different shared_ptrs: ";
        shared_ptr sp1{new Foo(testValue)};
        weak_ptr wp1 = sp1;

        shared_ptr sp2{new Foo(testValue)};
        weak_ptr wp2 = sp2;

        wp2 = std::move(wp1);

        std::cout << (1 == sp1.use_count()) << ", ";
        std::cout << (1 == sp2.use_count()) << ", ";
        std::cout << (1 == wp2.use_count()) << std::endl;
    }
    {
        std::cout << "reset: ";
        shared_ptr sp{new Foo{123}}, sp1 = sp, sp2 = sp1;
        weak_ptr wp = sp;
        std::cout << (3 == sp.use_count()) << ", ";
        std::cout << (3 == wp.use_count()) << ", ";
        wp.reset();
        std::cout << (3 == sp.use_count()) << ", ";
        std::cout << (0 == wp.use_count()) << std::endl;
    }
    {
        std::cout << "lock: ";
        shared_ptr sp{new Foo{123}};
        weak_ptr wp = sp;
        auto sp2 = wp.lock();
        std::cout << (2 == sp2.use_count()) << std::endl;
    }
    {
        std::cout << "swap: ";
        int val1 = 123;
        int val2 = 321;

        shared_ptr sp1{new Foo{val1}};
        shared_ptr sp2{new Foo{val2}};

        weak_ptr wp1 = sp1;
        weak_ptr wp2 = sp2;

        wp1.swap(wp2);
        auto sp = wp1.lock();
        std::cout << (val2 == sp->val) << std::endl;
    }
    {
        std::cout << "owner_before: ";
        auto sp1 = shared_ptr<Foo>{new Foo{123}};
        auto sp2 = shared_ptr<Foo>{new Foo{123}};
        std::cout << !sp1.owner_before(sp1) << ", ";
        std::cout << sp1.owner_before(sp2) << ", ";
        std::cout << !sp2.owner_before(sp1) << ", ";

        auto wp1 = sp1;
        auto wp2 = sp2;
        std::cout << !wp1.owner_before(wp1) << ", ";
        std::cout << wp1.owner_before(wp2) << ", ";
        std::cout << !wp2.owner_before(wp1) << std::endl;
    }
    std::cout << std::endl;
}

int main()
{
    test_shared_ptr();
    test_weak_ptr();
    return 0;
}
