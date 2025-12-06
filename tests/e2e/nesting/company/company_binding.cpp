#include "mirror_bridge.hpp"
#include "company.hpp"

// Bind all classes in dependency order: Address -> Company -> Employee
MIRROR_BRIDGE_MODULE(company,
    mirror_bridge::bind_class<Address>(m, "Address");
    mirror_bridge::bind_class<Company>(m, "Company");
    mirror_bridge::bind_class<Employee>(m, "Employee");
)
