#include "lua/mirror_bridge_lua.hpp"
#include "simple_class.hpp"

MIRROR_BRIDGE_LUA_MODULE(simple_mb_lua,
    mirror_bridge::lua::bind_class<SimpleClass>(L, "SimpleClass");
)
