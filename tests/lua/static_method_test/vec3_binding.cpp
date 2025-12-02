// Lua binding for Vec3 test class
#include "lua/mirror_bridge_lua.hpp"
#include "vec3.hpp"

MIRROR_BRIDGE_LUA_MODULE(vec3,
    mirror_bridge::lua::bind_class<Vec3>(L, "Vec3");
)
