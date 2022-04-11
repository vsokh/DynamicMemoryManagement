#include <iostream>
#include "string.hpp"

int main()
{
    string str{"hello"};
    for (auto ch : str) {
        std::cout << ch << std::endl;
    }
    return 0;
}
