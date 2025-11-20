#!/usr/bin/env python3
"""
Pure Python implementation of Calculator for comparison.
"""

import math


class Calculator:
    def __init__(self):
        pass

    # Basic operations
    def add(self, a, b):
        return a + b

    def subtract(self, a, b):
        return a - b

    def multiply(self, a, b):
        return a * b

    def divide(self, a, b):
        return a / b if b != 0 else 0.0

    # Power and roots
    def power(self, base, exponent):
        return math.pow(base, exponent)

    def square_root(self, x):
        return math.sqrt(x)

    def cube_root(self, x):
        return x ** (1/3)

    # Trigonometric functions
    def sin(self, x):
        return math.sin(x)

    def cos(self, x):
        return math.cos(x)

    def tan(self, x):
        return math.tan(x)

    # Logarithms
    def log(self, x):
        return math.log(x)

    def log10(self, x):
        return math.log10(x)

    # Advanced operations
    def factorial(self, n):
        if n <= 1:
            return 1.0
        result = 1.0
        for i in range(2, n + 1):
            result *= i
        return result

    def fibonacci(self, n):
        if n <= 1:
            return n

        prev, curr = 0, 1
        for _ in range(2, n + 1):
            prev, curr = curr, prev + curr
        return curr

    def is_prime(self, n):
        if n <= 1:
            return False
        if n <= 3:
            return True
        if n % 2 == 0 or n % 3 == 0:
            return False

        i = 5
        while i * i <= n:
            if n % i == 0 or n % (i + 2) == 0:
                return False
            i += 6
        return True

    def gcd(self, a, b):
        while b != 0:
            a, b = b, a % b
        return a

    def lcm(self, a, b):
        return (a * b) // self.gcd(a, b)

    def sum_array(self, numbers):
        return sum(numbers)

    def average(self, numbers):
        if not numbers:
            return 0.0
        return sum(numbers) / len(numbers)

    def std_dev(self, numbers):
        if not numbers:
            return 0.0

        mean = self.average(numbers)
        sum_sq_diff = sum((x - mean) ** 2 for x in numbers)
        return math.sqrt(sum_sq_diff / len(numbers))

    def max(self, numbers):
        if not numbers:
            return 0.0
        return max(numbers)

    def min(self, numbers):
        if not numbers:
            return 0.0
        return min(numbers)


if __name__ == '__main__':
    calc = Calculator()
    print(f"10 + 5 = {calc.add(10, 5)}")
    print(f"10! = {calc.factorial(10)}")
    print(f"Fib(20) = {calc.fibonacci(20)}")
    print(f"Is 17 prime? {calc.is_prime(17)}")
