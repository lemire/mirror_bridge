// Binding for Vec3 test class
#include "vec3_test.hpp"
#include "../../python/mirror_bridge_python.hpp"

MIRROR_BRIDGE_MODULE(vec3_test,
    mirror_bridge::bind_class<Vec3>(m, "Vec3");
)
