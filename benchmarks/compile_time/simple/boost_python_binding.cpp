#include <boost/python.hpp>
#include "simple_class.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(simple_bp) {
    class_<SimpleClass>("SimpleClass")
        .def(init<>())
        .def(init<int, double, std::string>())
        .def_readwrite("id", &SimpleClass::id)
        .def_readwrite("value", &SimpleClass::value)
        .def_readwrite("name", &SimpleClass::name)
        .def("set_id", &SimpleClass::set_id)
        .def("get_id", &SimpleClass::get_id)
        .def("set_value", &SimpleClass::set_value)
        .def("get_value", &SimpleClass::get_value)
        .def("compute", &SimpleClass::compute);
}
