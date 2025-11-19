#include "mirror_bridge.hpp"
#include "person.hpp"

MIRROR_BRIDGE_MODULE(people,
    mirror_bridge::bind_class<Person>(m, "Person");
)
