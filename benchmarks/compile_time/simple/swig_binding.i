%module simple_swig

%{
#include "simple_class.hpp"
%}

%include "std_string.i"

class SimpleClass {
public:
    int id;
    double value;
    std::string name;

    SimpleClass();
    SimpleClass(int i, double v, std::string n);

    void set_id(int i);
    int get_id() const;

    void set_value(double v);
    double get_value() const;

    double compute(double x);
};
