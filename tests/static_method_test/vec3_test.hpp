// Test class matching the blog Vec3 example
// Tests: static methods, const reference parameters
#pragma once

#include <cmath>

struct Vec3 {
    double x, y, z;

    Vec3() : x(0), y(0), z(0) {}
    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    // Method with const reference parameter - tests from_python with const T&
    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    double length() const {
        return std::sqrt(x*x + y*y + z*z);
    }

    Vec3 normalize() const {
        double len = length();
        if (len == 0) return Vec3();
        return Vec3(x/len, y/len, z/len);
    }

    // Static method - tests static binding
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

    // Another static method for testing
    static Vec3 add_static(const Vec3& a, const Vec3& b) {
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z);
    }
};
