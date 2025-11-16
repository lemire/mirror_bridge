// Test auto-discovery with multiple classes in one file
#include <string>
#include <cmath>

struct Point {
    double x, y;

    Point() : x(0), y(0) {}
    Point(double x_, double y_) : x(x_), y(y_) {}

    double distance_from_origin() const {
        return std::sqrt(x * x + y * y);
    }
};

struct Circle {
    Point center;
    double radius;

    Circle() : radius(0) {}
    Circle(double x, double y, double r) : center(x, y), radius(r) {}

    double area() const {
        return 3.14159 * radius * radius;
    }
};
