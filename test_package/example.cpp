#include "aeh/function_ptr.hh"
#include <type_traits>
#include <iostream>

int main()
{
    constexpr bool are_same = std::is_same<aeh::function_ptr<void()>, void(*)()>::value;
    std::cout << "function_ptr declares functions pointers as if by doing it manually: " << std::boolalpha << are_same << '\n';
}
