// Test Python single-header version
#include "../../single_header/mirror_bridge_python.hpp"
#include "robot.hpp"

MIRROR_BRIDGE_MODULE(robot_py,
    mirror_bridge::bind_class<Robot>(m, "Robot");
)
