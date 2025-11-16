#include "mirror_bridge.hpp"
#include "math_ops.hpp"

MIRROR_BRIDGE_MODULE(math_ops,
    mirror_bridge::bind_class<MathOps>(m, "MathOps");
)
