%module medium_swig

%{
#include "classes.hpp"
%}

%include "std_string.i"
%include "std_vector.i"

%template(Vector3DVector) std::vector<Vector3D>;
%template(IntVector) std::vector<int>;
%template(SceneNodeVector) std::vector<SceneNode>;
%template(CameraVector) std::vector<Camera>;
%template(LightVector) std::vector<Light>;
%template(RenderObjectVector) std::vector<RenderObject>;

struct Vector3D {
    double x, y, z;
    Vector3D();
    Vector3D(double x_, double y_, double z_);
    double length() const;
    Vector3D normalize() const;
};

struct Color {
    unsigned char r, g, b, a;
    Color();
    Color(unsigned char r_, unsigned char g_, unsigned char b_, unsigned char a_ = 255);
    unsigned int to_rgba() const;
};

struct Transform {
    Vector3D position;
    Vector3D rotation;
    Vector3D scale;
    Transform();
    void translate(double x, double y, double z);
    void rotate(double x, double y, double z);
};

struct Material {
    std::string name;
    Color diffuse;
    Color specular;
    double shininess;
    Material();
    Material(std::string n, Color d, Color s, double sh);
};

struct Mesh {
    std::string name;
    std::vector<Vector3D> vertices;
    std::vector<int> indices;
    Mesh();
    Mesh(std::string n);
    void add_vertex(const Vector3D& v);
    int vertex_count() const;
};

struct SceneNode {
    std::string name;
    Transform transform;
    std::vector<SceneNode> children;
    SceneNode();
    SceneNode(std::string n);
    void add_child(const SceneNode& node);
    int child_count() const;
};

struct Camera {
    std::string name;
    Transform transform;
    double fov;
    double near_plane;
    double far_plane;
    Camera();
    void look_at(const Vector3D& target);
    Vector3D get_forward() const;
};

struct Light {
    std::string name;
    Transform transform;
    Color color;
    double intensity;
    int type;
    Light();
    void set_intensity(double i);
};

struct RenderObject {
    std::string name;
    Mesh mesh;
    Material material;
    Transform transform;
    RenderObject();
    RenderObject(std::string n);
    void update_transform(const Transform& t);
};

struct Scene {
    std::string name;
    SceneNode root;
    std::vector<Camera> cameras;
    std::vector<Light> lights;
    std::vector<RenderObject> objects;
    Scene();
    Scene(std::string n);
    void add_camera(const Camera& cam);
    void add_light(const Light& light);
    void add_object(const RenderObject& obj);
    int total_objects() const;
};
