#pragma once
#include <string>

// Test class for method overloading via name mangling
struct Printer {
    std::string last_output;

    // Overloaded print methods
    void print(int value) {
        last_output = "int: " + std::to_string(value);
    }

    void print(double value) {
        last_output = "double: " + std::to_string(value);
    }

    void print(std::string value) {
        last_output = "string: " + value;
    }

    // Overloaded format methods
    std::string format(int a, int b) {
        return std::to_string(a) + "," + std::to_string(b);
    }

    std::string format(double a, double b) {
        return std::to_string(a) + ";" + std::to_string(b);
    }

    std::string format(std::string a, std::string b) {
        return a + " + " + b;
    }

    // Non-overloaded method for comparison
    std::string get_last() const {
        return last_output;
    }
};
