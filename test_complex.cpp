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
    if ( || ) {
        return false;
    }
    int i = 5;
    while (i * i <= num)  {
        if ( || ) {
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
        if () {
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
    std::cout << ("a + b = " + a + b) << std::endl;
    int fact5 = factorial(5);
    std::cout << ("Factorial of 5 = " + fact5) << std::endl;
    std::cout << ("Is 17 prime? " + isPrime(17)) << std::endl;
    std::cout << ("Is 20 prime? " + isPrime(20)) << std::endl;
    MathUtils math = MathUtils();
    std::cout << add(15, 7) << std::endl;
    std::cout << subtract(15, 7) << std::endl;
    std::cout << multiply(15, 7) << std::endl;
    std::cout << divide(15, 7) << std::endl;
    std::cout << power(2, 8) << std::endl;
    std::cout << divide(10, 0) << std::endl;
}

