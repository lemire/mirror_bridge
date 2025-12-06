#include "mirror_bridge.hpp"
#include "student.hpp"

// Bind all classes in dependency order: Address -> University -> Student
MIRROR_BRIDGE_MODULE(student,
    mirror_bridge::bind_class<Address>(m, "Address");
    mirror_bridge::bind_class<University>(m, "University");
    mirror_bridge::bind_class<Student>(m, "Student");
)
