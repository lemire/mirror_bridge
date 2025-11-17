#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include "benchmark_class.hpp"

namespace nb = nanobind;

NB_MODULE(bench_nb, m) {
    nb::class_<BenchmarkClass>(m, "BenchmarkClass")
        .def(nb::init<>())
        .def_rw("counter", &BenchmarkClass::counter)
        .def_rw("value", &BenchmarkClass::value)
        .def_rw("name", &BenchmarkClass::name)
        .def_rw("data", &BenchmarkClass::data)
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
