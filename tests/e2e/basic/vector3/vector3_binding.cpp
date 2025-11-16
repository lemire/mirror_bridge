// Auto-generated Python binding for Vector3 class
// This demonstrates the mirror_bridge single-header binding system

#include "mirror_bridge.hpp"
#include "vector3.hpp"

// Create the Python module with Vector3 binding
MIRROR_BRIDGE_MODULE(vector3,
    // Bind the Vector3 class using reflection
    // The bind_class function will automatically discover all members via C++26 reflection
    mirror_bridge::bind_class<Vector3>(m, "Vector3");
)
