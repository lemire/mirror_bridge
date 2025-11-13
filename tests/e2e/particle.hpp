#pragma once
#include <vector>
#include <array>

// Test class with container fields
// Demonstrates binding of std::vector and other containers
struct Particle {
    double mass = 1.0;

    // Dynamic container - std::vector
    std::vector<double> position{0.0, 0.0, 0.0};
    std::vector<double> velocity{0.0, 0.0, 0.0};

    // Fixed-size container - std::array
    std::array<double, 3> acceleration{0.0, 0.0, 0.0};
};
