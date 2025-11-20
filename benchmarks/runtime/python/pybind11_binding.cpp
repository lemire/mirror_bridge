#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "../shared/benchmark_class.hpp"

namespace py = pybind11;

PYBIND11_MODULE(bench_pb, m) {
    py::class_<BenchmarkClass>(m, "BenchmarkClass")
        .def(py::init<>())
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
