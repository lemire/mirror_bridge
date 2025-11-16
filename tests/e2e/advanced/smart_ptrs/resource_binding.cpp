#include "mirror_bridge.hpp"
#include "resource.hpp"

MIRROR_BRIDGE_MODULE(resource,
    mirror_bridge::bind_class<Data>(m, "Data");
    mirror_bridge::bind_class<ResourceManager>(m, "ResourceManager");
)
