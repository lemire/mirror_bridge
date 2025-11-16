// Simple benchmark: 1 class with basic functionality
#include <string>

struct SimpleClass {
    int id;
    double value;
    std::string name;

    SimpleClass() : id(0), value(0.0), name("") {}
    SimpleClass(int i, double v, std::string n) : id(i), value(v), name(n) {}

    void set_id(int i) { id = i; }
    int get_id() const { return id; }

    void set_value(double v) { value = v; }
    double get_value() const { return value; }

    double compute(double x) { return value * x; }
};
