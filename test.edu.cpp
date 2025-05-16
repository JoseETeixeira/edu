#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

int x = 42;
float y = 3.14;
std::string message = "Hello, World!";
int add(int a, int b) {
    return a + b;
}

class Calculator {
public:
    Calculator() {
        std::cout << "Calculator created" << std::endl;
    }
    int multiply(int a, int b) {
        return a * b;
    }
};

int main() {
    std::cout << message << std::endl;
    int result = add(x, 10);
    std::cout << result << std::endl;
    Calculator calc = Calculator();
    int product = calc.multiply(5, 7);
    std::cout << product << std::endl;
}

