// V8 binding for Vec3 with static methods
// This file is compiled into the test executable alongside the V8 test harness

#include "../../../javascript/mirror_bridge_v8.hpp"
#include "vec3.hpp"

// Define the V8 module initialization function
MIRROR_BRIDGE_V8_MODULE(vec3,
    mirror_bridge::v8_bindings::bind_class<Vec3>(isolate, exports, "Vec3");
)
