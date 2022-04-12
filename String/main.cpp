#include <iostream>
#include "string.hpp"

int main()
{
    string str{"hello "};
    str.append("world", 2, 3);

    std::cout << str.size() << std::endl;
    std::cout << str.capacity() << std::endl;
    std::cout << str.c_str() << std::endl;
    return 0;
}
