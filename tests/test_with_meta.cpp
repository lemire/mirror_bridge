#include <meta>
#include <iostream>

struct Point2D {
    double x;
    double y;
};

int main() {
    constexpr auto name = std::meta::identifier_of(^^Point2D);
    constexpr auto members = std::meta::nonstatic_data_members_of(^^Point2D);
    
    std::cout << "Class name: " << name << std::endl;
    std::cout << "Member count: " << members.size() << std::endl;
    
    return 0;
}
