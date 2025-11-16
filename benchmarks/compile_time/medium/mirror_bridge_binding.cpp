#include "mirror_bridge.hpp"
#include "classes.hpp"

MIRROR_BRIDGE_MODULE(medium_mb,
    mirror_bridge::bind_class<Vector3D>(m, "Vector3D");
    mirror_bridge::bind_class<Color>(m, "Color");
    mirror_bridge::bind_class<Transform>(m, "Transform");
    mirror_bridge::bind_class<Material>(m, "Material");
    mirror_bridge::bind_class<Mesh>(m, "Mesh");
    mirror_bridge::bind_class<SceneNode>(m, "SceneNode");
    mirror_bridge::bind_class<Camera>(m, "Camera");
    mirror_bridge::bind_class<Light>(m, "Light");
    mirror_bridge::bind_class<RenderObject>(m, "RenderObject");
    mirror_bridge::bind_class<Scene>(m, "Scene");
)
