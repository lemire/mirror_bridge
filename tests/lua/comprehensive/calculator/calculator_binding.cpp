#include "lua/mirror_bridge_lua.hpp"
#include "calculator.hpp"

MIRROR_BRIDGE_LUA_MODULE(calculator,
    mirror_bridge::lua::bind_class<Calculator>(L, "Calculator");
)
