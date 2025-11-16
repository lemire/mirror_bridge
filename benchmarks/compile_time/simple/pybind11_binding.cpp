#include <pybind11/pybind11.h>
#include "simple_class.hpp"

namespace py = pybind11;

PYBIND11_MODULE(simple_pb, m) {
    py::class_<SimpleClass>(m, "SimpleClass")
        .def(py::init<>())
        .def(py::init<int, double, std::string>())
        .def_readwrite("id", &SimpleClass::id)
        .def_readwrite("value", &SimpleClass::value)
        .def_readwrite("name", &SimpleClass::name)
        .def("set_id", &SimpleClass::set_id)
        .def("get_id", &SimpleClass::get_id)
        .def("set_value", &SimpleClass::set_value)
        .def("get_value", &SimpleClass::get_value)
        .def("compute", &SimpleClass::compute);
}
