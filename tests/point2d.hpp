#pragma once

// Simple 2D point class for testing mirror_bridge
struct Point2D {
    double x;
    double y;

    // Default constructor
    Point2D() : x(0), y(0) {}

    // Constructor with values
    Point2D(double x_, double y_) : x(x_), y(y_) {}

    // Distance from origin
    double distance_from_origin() const {
        return std::sqrt(x * x + y * y);
    }
};
