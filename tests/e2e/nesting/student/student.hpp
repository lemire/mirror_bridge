#pragma once
#include <string>
#include "university.hpp"  // Import University (which imports Address)

// Student depends on University which depends on Address
// This creates a dependency chain: Student -> University -> Address
struct Student {
    std::string name = "";
    int student_id = 0;
    University school;  // Nested class from different file
};
