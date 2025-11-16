#pragma once
#include <string>

// Test class for constructor parameter binding
struct Rectangle {
    double width;
    double height;
    std::string name;

    // Default constructor
    Rectangle() : width(0.0), height(0.0), name("unnamed") {}

    // 2-parameter constructor
    Rectangle(double w, double h) : width(w), height(h), name("rectangle") {}

    // 3-parameter constructor
    Rectangle(double w, double h, std::string n) : width(w), height(h), name(n) {}

    double area() const {
        return width * height;
    }

    double perimeter() const {
        return 2.0 * (width + height);
    }
};
