#include "other.hpp"
#include <cassert>
#include <iostream>

int main()
{
    std::cout << "Hi\n";
    auto const hello = foo();

    assert(hello == "hello");
    std::cout << hello << '\n';
    return 0;
}
