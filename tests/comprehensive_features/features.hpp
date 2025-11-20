#pragma once
#include <vector>
#include <string>
#include <memory>

// Enum test
enum class Color {
    Red = 0,
    Green = 1,
    Blue = 2
};

// Container test
struct VectorTest {
    std::vector<int> numbers;
    std::vector<std::string> names;

    int get_sum() const {
        int sum = 0;
        for (int n : numbers) sum += n;
        return sum;
    }

    int count_names() const {
        return names.size();
    }
};

// Smart pointer test
struct Resource {
    std::string name;
    int value = 0;

    std::string get_info() const {
        return name + ":" + std::to_string(value);
    }
};

struct ResourceHolder {
    std::unique_ptr<Resource> resource;

    std::string get_resource_info() const {
        if (resource) {
            return resource->get_info();
        }
        return "null";
    }
};

// Nested object test
struct Address {
    std::string street;
    std::string city;
    int zip = 0;
};

struct Person {
    std::string name;
    int age = 0;
    Address address;

    std::string get_full_address() const {
        return address.street + ", " + address.city + " " + std::to_string(address.zip);
    }
};

// Enum property test
struct ColoredShape {
    Color color = Color::Red;
    double size = 1.0;

    int get_color_value() const {
        return static_cast<int>(color);
    }
};
