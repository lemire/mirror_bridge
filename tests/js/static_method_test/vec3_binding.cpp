// JavaScript binding for Vec3 test class
#include "javascript/mirror_bridge_javascript.hpp"
#include "vec3.hpp"

MIRROR_BRIDGE_JS_MODULE(vec3,
    mirror_bridge::javascript::bind_class<Vec3>(env, m, "Vec3");
)
