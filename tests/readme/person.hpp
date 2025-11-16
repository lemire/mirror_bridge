// Example from README Quick Start section
#include <string>

struct Person {
    std::string name;
    int age;

    Person() = default;
    Person(std::string n, int a) : name(n), age(a) {}

    int birth_year(int current_year) { return current_year - age; }
};
