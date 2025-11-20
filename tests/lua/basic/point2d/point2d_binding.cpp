#include "lua/mirror_bridge_lua.hpp"
#include "point2d.hpp"

MIRROR_BRIDGE_LUA_MODULE(point2d,
    mirror_bridge::lua::bind_class<Point2D>(L, "Point2D");
)
