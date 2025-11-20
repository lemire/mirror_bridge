// Test Lua single-header version
#include "../../single_header/mirror_bridge_lua.hpp"
#include "robot.hpp"

MIRROR_BRIDGE_LUA_MODULE(robot_lua,
    mirror_bridge::lua::bind_class<Robot>(L, "Robot");
)
