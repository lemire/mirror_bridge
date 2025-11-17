#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include "simple_class.hpp"

namespace nb = nanobind;

NB_MODULE(simple_nb, m) {
    nb::class_<SimpleClass>(m, "SimpleClass")
        .def(nb::init<>())
        .def(nb::init<int, double, std::string>())
        .def_rw("id", &SimpleClass::id)
        .def_rw("value", &SimpleClass::value)
        .def_rw("name", &SimpleClass::name)
        .def("set_id", &SimpleClass::set_id)
        .def("get_id", &SimpleClass::get_id)
        .def("set_value", &SimpleClass::set_value)
        .def("get_value", &SimpleClass::get_value)
        .def("compute", &SimpleClass::compute);
}
