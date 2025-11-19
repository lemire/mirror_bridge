#include "mirror_bridge.hpp"
#include "calculator.hpp"

MIRROR_BRIDGE_MODULE(cpp_calc,
    mirror_bridge::bind_class<Calculator>(m, "Calculator");
)
