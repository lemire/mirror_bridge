#pragma once
#include <string>

// Nested class used as a member type
struct Address {
    std::string street = "";
    std::string city = "";
    int zip_code = 0;
};

// Main class with nested class member
// Demonstrates binding of classes that contain other bindable classes
struct Person {
    std::string name = "";
    int age = 0;
    Address address;  // Nested bindable class
};
