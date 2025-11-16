#include "mirror_bridge.hpp"
#include "calculator.hpp"

MIRROR_BRIDGE_MODULE(calculator,
    mirror_bridge::bind_class<Calculator>(m, "Calculator");
)
