#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "classes.hpp"

namespace py = pybind11;

PYBIND11_MODULE(medium_pb, m) {
    py::class_<Vector3D>(m, "Vector3D")
        .def(py::init<>())
        .def(py::init<double, double, double>())
        .def_readwrite("x", &Vector3D::x)
        .def_readwrite("y", &Vector3D::y)
        .def_readwrite("z", &Vector3D::z)
        .def("length", &Vector3D::length)
        .def("normalize", &Vector3D::normalize);

    py::class_<Color>(m, "Color")
        .def(py::init<>())
        .def(py::init<unsigned char, unsigned char, unsigned char, unsigned char>(),
             py::arg("r"), py::arg("g"), py::arg("b"), py::arg("a") = 255)
        .def_readwrite("r", &Color::r)
        .def_readwrite("g", &Color::g)
        .def_readwrite("b", &Color::b)
        .def_readwrite("a", &Color::a)
        .def("to_rgba", &Color::to_rgba);

    py::class_<Transform>(m, "Transform")
        .def(py::init<>())
        .def_readwrite("position", &Transform::position)
        .def_readwrite("rotation", &Transform::rotation)
        .def_readwrite("scale", &Transform::scale)
        .def("translate", &Transform::translate)
        .def("rotate", &Transform::rotate);

    py::class_<Material>(m, "Material")
        .def(py::init<>())
        .def(py::init<std::string, Color, Color, double>())
        .def_readwrite("name", &Material::name)
        .def_readwrite("diffuse", &Material::diffuse)
        .def_readwrite("specular", &Material::specular)
        .def_readwrite("shininess", &Material::shininess);

    py::class_<Mesh>(m, "Mesh")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def_readwrite("name", &Mesh::name)
        .def_readwrite("vertices", &Mesh::vertices)
        .def_readwrite("indices", &Mesh::indices)
        .def("add_vertex", &Mesh::add_vertex)
        .def("vertex_count", &Mesh::vertex_count);

    py::class_<SceneNode>(m, "SceneNode")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def_readwrite("name", &SceneNode::name)
        .def_readwrite("transform", &SceneNode::transform)
        .def_readwrite("children", &SceneNode::children)
        .def("add_child", &SceneNode::add_child)
        .def("child_count", &SceneNode::child_count);

    py::class_<Camera>(m, "Camera")
        .def(py::init<>())
        .def_readwrite("name", &Camera::name)
        .def_readwrite("transform", &Camera::transform)
        .def_readwrite("fov", &Camera::fov)
        .def_readwrite("near_plane", &Camera::near_plane)
        .def_readwrite("far_plane", &Camera::far_plane)
        .def("look_at", &Camera::look_at)
        .def("get_forward", &Camera::get_forward);

    py::enum_<Light::Type>(m, "LightType")
        .value("POINT", Light::POINT)
        .value("DIRECTIONAL", Light::DIRECTIONAL)
        .value("SPOT", Light::SPOT);

    py::class_<Light>(m, "Light")
        .def(py::init<>())
        .def_readwrite("name", &Light::name)
        .def_readwrite("transform", &Light::transform)
        .def_readwrite("color", &Light::color)
        .def_readwrite("intensity", &Light::intensity)
        .def_readwrite("type", &Light::type)
        .def("set_intensity", &Light::set_intensity);

    py::class_<RenderObject>(m, "RenderObject")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def_readwrite("name", &RenderObject::name)
        .def_readwrite("mesh", &RenderObject::mesh)
        .def_readwrite("material", &RenderObject::material)
        .def_readwrite("transform", &RenderObject::transform)
        .def("update_transform", &RenderObject::update_transform);

    py::class_<Scene>(m, "Scene")
        .def(py::init<>())
        .def(py::init<std::string>())
        .def_readwrite("name", &Scene::name)
        .def_readwrite("root", &Scene::root)
        .def_readwrite("cameras", &Scene::cameras)
        .def_readwrite("lights", &Scene::lights)
        .def_readwrite("objects", &Scene::objects)
        .def("add_camera", &Scene::add_camera)
        .def("add_light", &Scene::add_light)
        .def("add_object", &Scene::add_object)
        .def("total_objects", &Scene::total_objects);
}
