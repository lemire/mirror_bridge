#pragma once
#include <string>

// Level 3: Address (innermost)
struct Address {
    std::string street = "";
    std::string city = "";
    int zip_code = 0;
};

// Level 2: Company (contains Address)
struct Company {
    std::string name = "";
    Address headquarters;  // Nested Address
    int employee_count = 0;
};

// Level 1: Employee (contains Company which contains Address)
struct Employee {
    std::string name = "";
    int id = 0;
    Company employer;  // Nested Company (which has nested Address)
};
