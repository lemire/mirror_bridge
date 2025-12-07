// V8 binding for Point2D
// This file is compiled into the test executable alongside the V8 test harness

#include "../../../../javascript/mirror_bridge_v8.hpp"
#include "point2d.hpp"

// Define the V8 module initialization function
MIRROR_BRIDGE_V8_MODULE(point2d,
    mirror_bridge::v8_bindings::bind_class<Point2D>(isolate, exports, "Point2D");
)
