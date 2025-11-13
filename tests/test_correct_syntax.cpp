#include <iostream>

struct Point2D {
    double x;
    double y;
};

int main() {
    // Test with namespace prefix
    constexpr auto members = std::meta::nonstatic_data_members_of(^^Point2D);
    std::cout << "Member count: " << members.size() << std::endl;
    
    return 0;
}
