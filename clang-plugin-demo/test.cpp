#include <iostream>

// Good: follows snake_case
void calculate_sum(int a, int b) {
    std::cout << "Sum: " << (a + b) << std::endl;
}

// Bad: uses camelCase - plugin will warn about this
void calculateProduct(int a, int b) {
    std::cout << "Product: " << (a * b) << std::endl;
}

// Bad: uses PascalCase - plugin will warn about this
void CalculateDifference(int a, int b) {
    std::cout << "Difference: " << (a - b) << std::endl;
}

int main() {
    calculate_sum(5, 3);
    calculateProduct(5, 3);
    CalculateDifference(5, 3);
    return 0;
}
