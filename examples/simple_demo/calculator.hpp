// Simple C++ Calculator
// Fast mathematical operations

#pragma once
#include <cmath>
#include <vector>
#include <numeric>

class Calculator {
public:
    Calculator() {}

    // Basic operations
    double add(double a, double b) const {
        return a + b;
    }

    double subtract(double a, double b) const {
        return a - b;
    }

    double multiply(double a, double b) const {
        return a * b;
    }

    double divide(double a, double b) const {
        return b != 0 ? a / b : 0.0;
    }

    // Power and roots
    double power(double base, double exponent) const {
        return std::pow(base, exponent);
    }

    double square_root(double x) const {
        return std::sqrt(x);
    }

    double cube_root(double x) const {
        return std::cbrt(x);
    }

    // Trigonometric functions
    double sin(double x) const {
        return std::sin(x);
    }

    double cos(double x) const {
        return std::cos(x);
    }

    double tan(double x) const {
        return std::tan(x);
    }

    // Logarithms
    double log(double x) const {
        return std::log(x);
    }

    double log10(double x) const {
        return std::log10(x);
    }

    // Advanced operations
    double factorial(int n) const {
        if (n <= 1) return 1.0;
        double result = 1.0;
        for (int i = 2; i <= n; ++i) {
            result *= i;
        }
        return result;
    }

    // Compute Fibonacci number
    long long fibonacci(int n) const {
        if (n <= 1) return n;

        long long prev = 0, curr = 1;
        for (int i = 2; i <= n; ++i) {
            long long next = prev + curr;
            prev = curr;
            curr = next;
        }
        return curr;
    }

    // Check if number is prime
    bool is_prime(int n) const {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;

        for (int i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0)
                return false;
        }
        return true;
    }

    // Greatest common divisor
    int gcd(int a, int b) const {
        while (b != 0) {
            int temp = b;
            b = a % b;
            a = temp;
        }
        return a;
    }

    // Least common multiple
    int lcm(int a, int b) const {
        return (a * b) / gcd(a, b);
    }

    // Sum of array
    double sum_array(const std::vector<double>& numbers) const {
        return std::accumulate(numbers.begin(), numbers.end(), 0.0);
    }

    // Average of array
    double average(const std::vector<double>& numbers) const {
        if (numbers.empty()) return 0.0;
        return sum_array(numbers) / numbers.size();
    }

    // Standard deviation
    double std_dev(const std::vector<double>& numbers) const {
        if (numbers.empty()) return 0.0;

        double mean = average(numbers);
        double sum_sq_diff = 0.0;

        for (double num : numbers) {
            double diff = num - mean;
            sum_sq_diff += diff * diff;
        }

        return std::sqrt(sum_sq_diff / numbers.size());
    }

    // Find maximum in array
    double max(const std::vector<double>& numbers) const {
        if (numbers.empty()) return 0.0;
        return *std::max_element(numbers.begin(), numbers.end());
    }

    // Find minimum in array
    double min(const std::vector<double>& numbers) const {
        if (numbers.empty()) return 0.0;
        return *std::min_element(numbers.begin(), numbers.end());
    }
};
