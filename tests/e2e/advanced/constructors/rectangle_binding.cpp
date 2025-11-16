#include "mirror_bridge.hpp"
#include "rectangle.hpp"

MIRROR_BRIDGE_MODULE(rectangle,
    mirror_bridge::bind_class<Rectangle>(m, "Rectangle");
)
