// Test class for V8 bindings - static methods and const ref parameters
// (Same as Node.js test for parity)
#pragma once

#include <cmath>

struct Vec3 {
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    // Method with const reference parameter
    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    double length() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    // Static method with primitive parameter
    static double hot_loop(int n) {
        Vec3 direction(1, 1, 1);
        double dir_len = direction.length();
        double total = 0.0;

        for (int i = 0; i < n; ++i) {
            Vec3 v(i * 0.1, i * 0.2, i * 0.3);
            total += v.dot(direction) / dir_len;
        }
        return total;
    }

    // Static method with const ref parameters returning bound type
    static Vec3 add_vectors(const Vec3& a, const Vec3& b) {
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
};
