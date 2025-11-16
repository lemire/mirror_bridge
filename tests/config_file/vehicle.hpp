// Test config-file based generation
#include <string>

struct Vehicle {
    std::string brand;
    int year;
    double mileage;

    Vehicle() : year(0), mileage(0.0) {}
    Vehicle(std::string b, int y, double m) : brand(b), year(y), mileage(m) {}

    int age(int current_year) const {
        return current_year - year;
    }
};
