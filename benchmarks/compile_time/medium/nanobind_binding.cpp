#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include "classes.hpp"

namespace nb = nanobind;

NB_MODULE(medium_nb, m) {
    nb::class_<Vector3D>(m, "Vector3D")
        .def(nb::init<>())
        .def(nb::init<double, double, double>())
        .def_rw("x", &Vector3D::x)
        .def_rw("y", &Vector3D::y)
        .def_rw("z", &Vector3D::z)
        .def("length", &Vector3D::length)
        .def("normalize", &Vector3D::normalize);

    nb::class_<Color>(m, "Color")
        .def(nb::init<>())
        .def(nb::init<unsigned char, unsigned char, unsigned char, unsigned char>(),
             nb::arg("r"), nb::arg("g"), nb::arg("b"), nb::arg("a") = 255)
        .def_rw("r", &Color::r)
        .def_rw("g", &Color::g)
        .def_rw("b", &Color::b)
        .def_rw("a", &Color::a)
        .def("to_rgba", &Color::to_rgba);

    nb::class_<Transform>(m, "Transform")
        .def(nb::init<>())
        .def_rw("position", &Transform::position)
        .def_rw("rotation", &Transform::rotation)
        .def_rw("scale", &Transform::scale)
        .def("translate", &Transform::translate)
        .def("rotate", &Transform::rotate);

    nb::class_<Material>(m, "Material")
        .def(nb::init<>())
        .def(nb::init<std::string, Color, Color, double>())
        .def_rw("name", &Material::name)
        .def_rw("diffuse", &Material::diffuse)
        .def_rw("specular", &Material::specular)
        .def_rw("shininess", &Material::shininess);

    nb::class_<Mesh>(m, "Mesh")
        .def(nb::init<>())
        .def(nb::init<std::string>())
        .def_rw("name", &Mesh::name)
        .def_rw("vertices", &Mesh::vertices)
        .def_rw("indices", &Mesh::indices)
        .def("add_vertex", &Mesh::add_vertex)
        .def("vertex_count", &Mesh::vertex_count);

    nb::class_<SceneNode>(m, "SceneNode")
        .def(nb::init<>())
        .def(nb::init<std::string>())
        .def_rw("name", &SceneNode::name)
        .def_rw("transform", &SceneNode::transform)
        .def_rw("children", &SceneNode::children)
        .def("add_child", &SceneNode::add_child)
        .def("child_count", &SceneNode::child_count);

    nb::class_<Camera>(m, "Camera")
        .def(nb::init<>())
        .def_rw("name", &Camera::name)
        .def_rw("transform", &Camera::transform)
        .def_rw("fov", &Camera::fov)
        .def_rw("near_plane", &Camera::near_plane)
        .def_rw("far_plane", &Camera::far_plane)
        .def("look_at", &Camera::look_at)
        .def("get_forward", &Camera::get_forward);

    // Note: Light::Type enum removed (simplified to int for fair comparison)

    nb::class_<Light>(m, "Light")
        .def(nb::init<>())
        .def_rw("name", &Light::name)
        .def_rw("transform", &Light::transform)
        .def_rw("color", &Light::color)
        .def_rw("intensity", &Light::intensity)
        .def_rw("type", &Light::type)
        .def("set_intensity", &Light::set_intensity);

    nb::class_<RenderObject>(m, "RenderObject")
        .def(nb::init<>())
        .def(nb::init<std::string>())
        .def_rw("name", &RenderObject::name)
        .def_rw("mesh", &RenderObject::mesh)
        .def_rw("material", &RenderObject::material)
        .def_rw("transform", &RenderObject::transform)
        .def("update_transform", &RenderObject::update_transform);

    nb::class_<Scene>(m, "Scene")
        .def(nb::init<>())
        .def(nb::init<std::string>())
        .def_rw("name", &Scene::name)
        .def_rw("root", &Scene::root)
        .def_rw("cameras", &Scene::cameras)
        .def_rw("lights", &Scene::lights)
        .def_rw("objects", &Scene::objects)
        .def("add_camera", &Scene::add_camera)
        .def("add_light", &Scene::add_light)
        .def("add_object", &Scene::add_object)
        .def("total_objects", &Scene::total_objects);
}
