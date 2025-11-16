#pragma once
#include <string>
#include "address.hpp"  // Import Address from separate file

// University depends on Address (cross-file dependency)
struct University {
    std::string name = "";
    Address location;  // Nested class from different file
    int student_count = 0;
};
