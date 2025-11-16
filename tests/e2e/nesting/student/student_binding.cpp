#include "mirror_bridge.hpp"
#include "student.hpp"

// Only bind Student - University and Address are automatically handled
// as nested types (converted to Python dicts)
MIRROR_BRIDGE_MODULE(student,
    mirror_bridge::bind_class<Student>(m, "Student");
)
