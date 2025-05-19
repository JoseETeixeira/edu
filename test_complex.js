#!/usr/bin/env node

function factorial(n) {
  if (n <= 1) {
    return 1;
  }
  return n * factorial(n - 1);
}

function isPrime(num) {
  if (num <= 1) {
    return false;
  }
  if (num <= 3) {
    return true;
  }
  if (num % 2 === 0 || num % 3 === 0) {
    return false;
  }
  let i = 5;
  while (i * i <= num) {
    if (num % i === 0 || num % (i + 2) === 0) {
      return false;
    }
    i += 6;
  }
  return true;
}

class MathUtils {
  constructor() {
    console.log("MathUtils initialized");
  }

  add(x, y) {
    return x + y;
  }

  subtract(x, y) {
    return x - y;
  }

  multiply(x, y) {
    return x * y;
  }

  divide(x, y) {
    if (y === 0) {
      console.log("Error: Division by zero");
      return 0;
    }
    return x / y;
  }

  power(base, exponent) {
    let result = 1;
    for (let i = 0; i < exponent; i++) {
      result *= base;
    }
    return result;
  }
}

function main() {
  const a = 10;
  const b = 20;
  const pi = 3.14159;
  const greeting = "Hello, edu language!";

  console.log(greeting);
  console.log(`a + b = ${a + b}`);

  const fact5 = factorial(5);
  console.log(`Factorial of 5 = ${fact5}`);

  console.log(`Is 17 prime? ${isPrime(17)}`);
  console.log(`Is 20 prime? ${isPrime(20)}`);

  const math = new MathUtils();

  console.log(`Addition: 15 + 7 = ${math.add(15, 7)}`);
  console.log(`Subtraction: 15 - 7 = ${math.subtract(15, 7)}`);
  console.log(`Multiplication: 15 * 7 = ${math.multiply(15, 7)}`);

  const div = math.divide(15, 7);
  console.log(`Division: 15 / 7 = ${div.toFixed(2)}`);

  console.log(`Power: 2^8 = ${math.power(2, 8)}`);

  const div0 = math.divide(10, 0);
  console.log(`Division by zero test: ${div0}`);
}

main();
