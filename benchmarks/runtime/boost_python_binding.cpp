#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "benchmark_class.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(bench_bp) {
    class_<std::vector<double>>("DoubleList")
        .def(vector_indexing_suite<std::vector<double>>());

    class_<BenchmarkClass>("BenchmarkClass")
        .def(init<>())
        .def_readwrite("counter", &BenchmarkClass::counter)
        .def_readwrite("value", &BenchmarkClass::value)
        .def_readwrite("name", &BenchmarkClass::name)
        .def_readwrite("data", &BenchmarkClass::data)
        .def("null_call", &BenchmarkClass::null_call)
        .def("add_int", &BenchmarkClass::add_int)
        .def("multiply_double", &BenchmarkClass::multiply_double)
        .def("concat_string", &BenchmarkClass::concat_string)
        .def("get_string", &BenchmarkClass::get_string)
        .def("set_string", &BenchmarkClass::set_string)
        .def("add_to_vector", &BenchmarkClass::add_to_vector)
        .def("get_vector", &BenchmarkClass::get_vector)
        .def("set_vector", &BenchmarkClass::set_vector)
        .def("get_counter", &BenchmarkClass::get_counter)
        .def("set_counter", &BenchmarkClass::set_counter);
}
