#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

int main() {
    std::cout << factorial(5) << std::endl;
}

