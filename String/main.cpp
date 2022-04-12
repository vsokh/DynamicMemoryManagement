#include <iostream>
#include "string.hpp"

int main()
{
    string s{'a'};
    std::cout << s.size() << std::endl;
    std::cout << s.capacity() << std::endl;
    return 0;
}
