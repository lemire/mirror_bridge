#include "../../mirror_bridge.hpp"
#include "document.hpp"

MIRROR_BRIDGE_MODULE(document_bindings,
    mirror_bridge::bind_class<Document>(m, "Document");
)
