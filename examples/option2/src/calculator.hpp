#pragma once
#include <string>
#include <stdexcept>

struct Calculator {
    double value = 0.0;

    // Basic arithmetic methods
    double add(double x) {
        value += x;
        return value;
    }

    double subtract(double x) {
        value -= x;
        return value;
    }

    double multiply(double x) {
        value *= x;
        return value;
    }

    double divide(double x) {
        if (x == 0.0) {
            throw std::runtime_error("Division by zero");
        }
        value /= x;
        return value;
    }

    // Const method that doesn't modify state
    double get_value() const {
        return value;
    }

    // Method with multiple parameters
    double compute(double x, double y) {
        value = (x + y) * 2.0;
        return value;
    }

    // Void method
    void reset() {
        value = 0.0;
    }

    // Method returning string
    std::string to_string() const {
        return "Calculator(value=" + std::to_string(value) + ")";
    }
};
