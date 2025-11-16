#include "mirror_bridge.hpp"
#include "document.hpp"

MIRROR_BRIDGE_MODULE(document,
    mirror_bridge::bind_class<Document>(m, "Document");
)
