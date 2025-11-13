#include <iostream>

struct Point2D {
    double x;
    double y;
};

int main() {
    // Try without std::meta prefix
    constexpr auto members = nonstatic_data_members_of(^^Point2D);
    std::cout << "Member count: " << members.size() << std::endl;
    
    return 0;
}
