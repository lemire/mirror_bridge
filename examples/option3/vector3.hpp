#pragma once

#include <cmath>

// Simple 3D vector class for testing mirror_bridge
struct Vector3 {
    double x;
    double y;
    double z;

    // Default constructor
    Vector3() : x(0), y(0), z(0) {}

    // Constructor with values
    Vector3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

    // Compute the length (magnitude) of the vector
    double length() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Compute squared length (faster, no sqrt)
    double length_squared() const {
        return x * x + y * y + z * z;
    }

    // Normalize the vector (make length = 1)
    void normalize() {
        double len = length();
        if (len > 0) {
            x /= len;
            y /= len;
            z /= len;
        }
    }

    // Dot product with another vector
    double dot(const Vector3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};
