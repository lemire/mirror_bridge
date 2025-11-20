#include "lua/mirror_bridge_lua.hpp"
#include "medium_class.hpp"

MIRROR_BRIDGE_LUA_MODULE(medium_mb_lua,
    mirror_bridge::lua::bind_class<MediumClass>(L, "MediumClass");
)
