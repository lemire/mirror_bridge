#include "mirror_bridge.hpp"
#include "person.hpp"

MIRROR_BRIDGE_MODULE(person,
    mirror_bridge::bind_class<Address>(m, "Address");
    mirror_bridge::bind_class<Person>(m, "Person");
)
