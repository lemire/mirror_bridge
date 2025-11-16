#pragma once

// Test class for variadic parameter support (>2 parameters)
struct MathOps {
    double value = 0.0;

    // 3 parameters
    double add3(double a, double b, double c) {
        value = a + b + c;
        return value;
    }

    // 4 parameters
    double multiply4(double a, double b, double c, double d) {
        value = a * b * c * d;
        return value;
    }

    // 5 parameters
    double sum5(double a, double b, double c, double d, double e) {
        value = a + b + c + d + e;
        return value;
    }

    // Mixed types with 3 parameters
    std::string format3(std::string prefix, int num, std::string suffix) {
        return prefix + std::to_string(num) + suffix;
    }

    // 6 parameters with different types
    double weighted_sum(double a, double w1, double b, double w2, double c, double w3) {
        value = a * w1 + b * w2 + c * w3;
        return value;
    }
};
