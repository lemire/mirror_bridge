#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include "classes.hpp"

using namespace boost::python;

BOOST_PYTHON_MODULE(medium_bp) {
    class_<Vector3D>("Vector3D")
        .def(init<>())
        .def(init<double, double, double>())
        .def_readwrite("x", &Vector3D::x)
        .def_readwrite("y", &Vector3D::y)
        .def_readwrite("z", &Vector3D::z)
        .def("length", &Vector3D::length)
        .def("normalize", &Vector3D::normalize);

    class_<Color>("Color")
        .def(init<>())
        .def(init<unsigned char, unsigned char, unsigned char, unsigned char>())
        .def_readwrite("r", &Color::r)
        .def_readwrite("g", &Color::g)
        .def_readwrite("b", &Color::b)
        .def_readwrite("a", &Color::a)
        .def("to_rgba", &Color::to_rgba);

    class_<Transform>("Transform")
        .def(init<>())
        .def_readwrite("position", &Transform::position)
        .def_readwrite("rotation", &Transform::rotation)
        .def_readwrite("scale", &Transform::scale)
        .def("translate", &Transform::translate)
        .def("rotate", &Transform::rotate);

    class_<Material>("Material")
        .def(init<>())
        .def(init<std::string, Color, Color, double>())
        .def_readwrite("name", &Material::name)
        .def_readwrite("diffuse", &Material::diffuse)
        .def_readwrite("specular", &Material::specular)
        .def_readwrite("shininess", &Material::shininess);

    class_<std::vector<Vector3D>>("Vector3DList")
        .def(vector_indexing_suite<std::vector<Vector3D>>());

    class_<std::vector<int>>("IntList")
        .def(vector_indexing_suite<std::vector<int>>());

    class_<Mesh>("Mesh")
        .def(init<>())
        .def(init<std::string>())
        .def_readwrite("name", &Mesh::name)
        .def_readwrite("vertices", &Mesh::vertices)
        .def_readwrite("indices", &Mesh::indices)
        .def("add_vertex", &Mesh::add_vertex)
        .def("vertex_count", &Mesh::vertex_count);

    class_<std::vector<SceneNode>>("SceneNodeList")
        .def(vector_indexing_suite<std::vector<SceneNode>>());

    class_<SceneNode>("SceneNode")
        .def(init<>())
        .def(init<std::string>())
        .def_readwrite("name", &SceneNode::name)
        .def_readwrite("transform", &SceneNode::transform)
        .def_readwrite("children", &SceneNode::children)
        .def("add_child", &SceneNode::add_child)
        .def("child_count", &SceneNode::child_count);

    class_<Camera>("Camera")
        .def(init<>())
        .def_readwrite("name", &Camera::name)
        .def_readwrite("transform", &Camera::transform)
        .def_readwrite("fov", &Camera::fov)
        .def_readwrite("near_plane", &Camera::near_plane)
        .def_readwrite("far_plane", &Camera::far_plane)
        .def("look_at", &Camera::look_at)
        .def("get_forward", &Camera::get_forward);

    enum_<Light::Type>("LightType")
        .value("POINT", Light::POINT)
        .value("DIRECTIONAL", Light::DIRECTIONAL)
        .value("SPOT", Light::SPOT);

    class_<Light>("Light")
        .def(init<>())
        .def_readwrite("name", &Light::name)
        .def_readwrite("transform", &Light::transform)
        .def_readwrite("color", &Light::color)
        .def_readwrite("intensity", &Light::intensity)
        .def_readwrite("type", &Light::type)
        .def("set_intensity", &Light::set_intensity);

    class_<RenderObject>("RenderObject")
        .def(init<>())
        .def(init<std::string>())
        .def_readwrite("name", &RenderObject::name)
        .def_readwrite("mesh", &RenderObject::mesh)
        .def_readwrite("material", &RenderObject::material)
        .def_readwrite("transform", &RenderObject::transform)
        .def("update_transform", &RenderObject::update_transform);

    class_<std::vector<Camera>>("CameraList")
        .def(vector_indexing_suite<std::vector<Camera>>());

    class_<std::vector<Light>>("LightList")
        .def(vector_indexing_suite<std::vector<Light>>());

    class_<std::vector<RenderObject>>("RenderObjectList")
        .def(vector_indexing_suite<std::vector<RenderObject>>());

    class_<Scene>("Scene")
        .def(init<>())
        .def(init<std::string>())
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
