// Test JavaScript single-header version
#include "../../single_header/mirror_bridge_javascript.hpp"
#include "robot.hpp"

MIRROR_BRIDGE_JS_MODULE(robot_js,
    mirror_bridge::javascript::bind_class<Robot>(env, m, "Robot");
)
