%module bench_swig

%{
#include "benchmark_class.hpp"
%}

%include "std_string.i"
%include "std_vector.i"

%template(DoubleVector) std::vector<double>;

class BenchmarkClass {
public:
    int counter;
    double value;
    std::string name;
    std::vector<double> data;

    BenchmarkClass();

    void null_call();
    int add_int(int x);
    double multiply_double(double x);
    std::string concat_string(const std::string& s);
    std::string get_string() const;
    void set_string(const std::string& s);
    void add_to_vector(double x);
    std::vector<double> get_vector() const;
    void set_vector(const std::vector<double>& v);
    int get_counter() const;
    void set_counter(int c);
};
