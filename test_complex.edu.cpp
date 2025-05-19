#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <functional>

int a = 10;

int b = 20;

float pi = 3.14159;

std::string greeting = "Hello, edu language!";

int factorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

bool isPrime(int num) {
    if (num <= 1) {
        return false;
    }
    if (num <= 3) {
        return true;
    }
    if (num % 2 == 0 || num % 3 == 0) {
        return false;
    }
    int i = 5;
    while (i * i <= num)  {
        if (num % i == 0 || num % i + 2 == 0) {
            return false;
        }
        i = i + 6;
    }
    return true;
}

class MathUtils {
public:
    MathUtils() {
        std::cout << "MathUtils initialized" << std::endl;
    }
    int add(int x, int y) {
        return x + y;
    }
    int subtract(int x, int y) {
        return x - y;
    }
    int multiply(int x, int y) {
        return x * y;
    }
    float divide(int x, int y) {
        if (y == 0) {
            std::cout << "Error: Division by zero" << std::endl;
            return 0;
        }
        return x / y;
    }
    int power(int base, int exponent) {
        int result = 1;
        int i = 0;
        while (i < exponent)  {
            result = result * base;
            i = i + 1;
        }
        return result;
    }
};

int main() {
    std::cout << greeting << std::endl;
    std::cout << "a + b = " << a + b << std::endl;
    int fact5 = factorial(5);
    std::cout << "Factorial of 5 = " << fact5 << std::endl;
    std::cout << "Is 17 prime? " << (isPrime(17) ? "true" : "false") << std::endl;
    std::cout << "Is 20 prime? " << (isPrime(20) ? "true" : "false") << std::endl;
    MathUtils math = MathUtils();
    std::cout << "Addition: 15 + 7 = " << math.add(15, 7) << std::endl;
    std::cout << "Subtraction: 15 - 7 = " << math.subtract(15, 7) << std::endl;
    std::cout << "Multiplication: 15 * 7 = " << math.multiply(15, 7) << std::endl;
    std::cout << "Division: 15 / 7 = " << math.divide(15, 7) << std::endl;
    std::cout << "Power: 2^8 = " << math.power(2, 8) << std::endl;
    std::cout << "Division by zero test: " << math.divide(10, 0) << std::endl;
}

