#!/usr/bin/env python3

def factorial(n):
    if n <= 1:
        return 1
    return n * factorial(n - 1)

def is_prime(num):
    if num <= 1:
        return False
    if num <= 3:
        return True
    if num % 2 == 0 or num % 3 == 0:
        return False
    i = 5
    while i * i <= num:
        if num % i == 0 or num % (i + 2) == 0:
            return False
        i += 6
    return True

class MathUtils:
    def __init__(self):
        print("MathUtils initialized")

    def add(self, x, y):
        return x + y

    def subtract(self, x, y):
        return x - y

    def multiply(self, x, y):
        return x * y

    def divide(self, x, y):
        if y == 0:
            print("Error: Division by zero")
            return 0
        return x / y

    def power(self, base, exponent):
        result = 1
        for _ in range(exponent):
            result *= base
        return result

def main():
    a = 10
    b = 20
    pi = 3.14159
    greeting = "Hello, edu language!"

    print(greeting)
    print(f"a + b = {a + b}")

    fact5 = factorial(5)
    print(f"Factorial of 5 = {fact5}")

    print(f"Is 17 prime? {str(is_prime(17)).lower()}")
    print(f"Is 20 prime? {str(is_prime(20)).lower()}")

    math = MathUtils()

    print(f"Addition: 15 + 7 = {math.add(15, 7)}")
    print(f"Subtraction: 15 - 7 = {math.subtract(15, 7)}")
    print(f"Multiplication: 15 * 7 = {math.multiply(15, 7)}")

    div = math.divide(15, 7)
    print(f"Division: 15 / 7 = {div:.2f}")

    print(f"Power: 2^8 = {math.power(2, 8)}")

    div0 = math.divide(10, 0)
    print(f"Division by zero test: {div0}")

if __name__ == "__main__":
    main()