// Medium benchmark: 10 interconnected classes (realistic project)
#include <string>
#include <vector>

// Base types
struct Vector3D {
    double x, y, z;
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}
    double length() const;
    Vector3D normalize() const;
};

struct Color {
    unsigned char r, g, b, a;
    Color() : r(0), g(0), b(0), a(255) {}
    Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255)
        : r(r_), g(g_), b(b_), a(a_) {}
    unsigned int to_rgba() const;
};

struct Transform {
    Vector3D position;
    Vector3D rotation;
    Vector3D scale;
    Transform() : scale(1, 1, 1) {}
    void translate(double x, double y, double z);
    void rotate(double x, double y, double z);
};

// Scene graph
struct Material {
    std::string name;
    Color diffuse;
    Color specular;
    double shininess;
    Material() : shininess(32.0) {}
    Material(std::string n, Color d, Color s, double sh)
        : name(n), diffuse(d), specular(s), shininess(sh) {}
};

struct Mesh {
    std::string name;
    std::vector<Vector3D> vertices;
    std::vector<int> indices;
    Mesh() {}
    Mesh(std::string n) : name(n) {}
    void add_vertex(const Vector3D& v);
    int vertex_count() const;
};

struct SceneNode {
    std::string name;
    Transform transform;
    std::vector<SceneNode> children;
    SceneNode() {}
    SceneNode(std::string n) : name(n) {}
    void add_child(const SceneNode& node);
    int child_count() const;
};

struct Camera {
    std::string name;
    Transform transform;
    double fov;
    double near_plane;
    double far_plane;
    Camera() : fov(60.0), near_plane(0.1), far_plane(1000.0) {}
    void look_at(const Vector3D& target);
    Vector3D get_forward() const;
};

struct Light {
    std::string name;
    Transform transform;
    Color color;
    double intensity;
    enum Type { POINT, DIRECTIONAL, SPOT };
    Type type;
    Light() : intensity(1.0), type(POINT) {}
    void set_intensity(double i);
};

struct RenderObject {
    std::string name;
    Mesh mesh;
    Material material;
    Transform transform;
    RenderObject() {}
    RenderObject(std::string n) : name(n) {}
    void update_transform(const Transform& t);
};

struct Scene {
    std::string name;
    SceneNode root;
    std::vector<Camera> cameras;
    std::vector<Light> lights;
    std::vector<RenderObject> objects;
    Scene() {}
    Scene(std::string n) : name(n), root(n + "_root") {}
    void add_camera(const Camera& cam);
    void add_light(const Light& light);
    void add_object(const RenderObject& obj);
    int total_objects() const;
};

// Implementations (inline for simplicity)
inline double Vector3D::length() const {
    return std::sqrt(x*x + y*y + z*z);
}

inline Vector3D Vector3D::normalize() const {
    double len = length();
    return len > 0 ? Vector3D(x/len, y/len, z/len) : Vector3D();
}

inline unsigned int Color::to_rgba() const {
    return (r << 24) | (g << 16) | (b << 8) | a;
}

inline void Transform::translate(double x, double y, double z) {
    position.x += x; position.y += y; position.z += z;
}

inline void Transform::rotate(double x, double y, double z) {
    rotation.x += x; rotation.y += y; rotation.z += z;
}

inline void Mesh::add_vertex(const Vector3D& v) {
    vertices.push_back(v);
}

inline int Mesh::vertex_count() const {
    return static_cast<int>(vertices.size());
}

inline void SceneNode::add_child(const SceneNode& node) {
    children.push_back(node);
}

inline int SceneNode::child_count() const {
    return static_cast<int>(children.size());
}

inline void Camera::look_at(const Vector3D& target) {
    // Simplified look-at
    transform.position = target;
}

inline Vector3D Camera::get_forward() const {
    return Vector3D(0, 0, -1); // Simplified
}

inline void Light::set_intensity(double i) {
    intensity = i;
}

inline void RenderObject::update_transform(const Transform& t) {
    transform = t;
}

inline void Scene::add_camera(const Camera& cam) {
    cameras.push_back(cam);
}

inline void Scene::add_light(const Light& light) {
    lights.push_back(light);
}

inline void Scene::add_object(const RenderObject& obj) {
    objects.push_back(obj);
}

inline int Scene::total_objects() const {
    return static_cast<int>(objects.size());
}
