#include "javascript/mirror_bridge_javascript.hpp"
#include "medium_class.hpp"

MIRROR_BRIDGE_JS_MODULE(medium_mb_js,
    mirror_bridge::javascript::bind_class<MediumClass>(env, m, "MediumClass");
)
