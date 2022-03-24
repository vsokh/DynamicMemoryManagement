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
    SharedPointer<const S> sp1{new S};
    return 0;
}