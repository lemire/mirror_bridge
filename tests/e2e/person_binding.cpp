#include "../../mirror_bridge.hpp"
#include "person.hpp"

MIRROR_BRIDGE_MODULE(person_bindings,
    mirror_bridge::bind_class<Person>(m, "Person");
)
