// Template binding example
// Shows how to bind explicit template instantiations

#include "python/mirror_bridge_python.hpp"
#include "container.hpp"

MIRROR_BRIDGE_MODULE(container,
    // Bind each instantiation with a descriptive name
    mirror_bridge::bind_class<Container<int>>(m, "ContainerInt");
    mirror_bridge::bind_class<Container<double>>(m, "ContainerDouble");
)
