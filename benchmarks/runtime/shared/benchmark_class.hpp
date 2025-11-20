// Shared C++ class for runtime benchmarks
#include <string>
#include <vector>

struct BenchmarkClass {
    int counter;
    double value;
    std::string name;
    std::vector<double> data;

    BenchmarkClass() : counter(0), value(0.0), name("bench") {}

    // Null call - empty method (measures pure overhead)
    void null_call() {}

    // Arithmetic operations
    int add_int(int x) { return counter + x; }
    double multiply_double(double x) { return value * x; }

    // String operations
    std::string concat_string(const std::string& s) { return name + s; }
    std::string get_string() const { return name; }
    void set_string(const std::string& s) { name = s; }

    // Vector operations
    void add_to_vector(double x) { data.push_back(x); }
    std::vector<double> get_vector() const { return data; }
    void set_vector(const std::vector<double>& v) { data = v; }

    // Attribute access
    int get_counter() const { return counter; }
    void set_counter(int c) { counter = c; }
};
