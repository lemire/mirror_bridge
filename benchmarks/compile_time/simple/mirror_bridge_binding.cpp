#include "mirror_bridge.hpp"
#include "simple_class.hpp"

MIRROR_BRIDGE_MODULE(simple_mb,
    mirror_bridge::bind_class<SimpleClass>(m, "SimpleClass");
)
