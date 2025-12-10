#include <iostream>

int main() {
    constexpr int a = 42;
    constexpr int b = a * 2;
    std::cout << "Result: " << b << std::endl;
    return 0;
}