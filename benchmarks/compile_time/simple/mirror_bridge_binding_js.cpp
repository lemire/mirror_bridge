#include "javascript/mirror_bridge_javascript.hpp"
#include "simple_class.hpp"

MIRROR_BRIDGE_JS_MODULE(simple_mb_js,
    mirror_bridge::javascript::bind_class<SimpleClass>(env, m, "SimpleClass");
)
