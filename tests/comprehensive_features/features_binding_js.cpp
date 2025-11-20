// Manual binding for JavaScript (without ResourceHolder due to smart pointer issue)
#include "javascript/mirror_bridge_javascript.hpp"
#include "features.hpp"

MIRROR_BRIDGE_JS_MODULE(features_js,
    mirror_bridge::javascript::bind_class<VectorTest>(env, m, "VectorTest");
    mirror_bridge::javascript::bind_class<Resource>(env, m, "Resource");
    // mirror_bridge::javascript::bind_class<ResourceHolder>(env, m, "ResourceHolder");  // TODO: Fix smart pointer support
    mirror_bridge::javascript::bind_class<Address>(env, m, "Address");
    mirror_bridge::javascript::bind_class<Person>(env, m, "Person");
    mirror_bridge::javascript::bind_class<ColoredShape>(env, m, "ColoredShape");
)
