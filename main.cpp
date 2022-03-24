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
    SharedPointer<S> sp1{nullptr};
    std::cout << sp1.use_count() << std::endl;
    return 0;
}