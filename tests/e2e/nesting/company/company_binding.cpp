#include "mirror_bridge.hpp"
#include "company.hpp"

MIRROR_BRIDGE_MODULE(company,
    mirror_bridge::bind_class<Employee>(m, "Employee");
)
