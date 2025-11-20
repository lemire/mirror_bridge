#include "javascript/mirror_bridge_javascript.hpp"
#include "point2d.hpp"

MIRROR_BRIDGE_JS_MODULE(point2d,
    mirror_bridge::javascript::bind_class<Point2D>(env, m, "Point2D");
)
