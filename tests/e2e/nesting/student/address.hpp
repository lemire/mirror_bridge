#pragma once
#include <string>

// Address class defined in its own file
struct Address {
    std::string street = "";
    std::string city = "";
    std::string country = "";
    int zip_code = 0;
};
