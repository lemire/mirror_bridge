#include "javascript/mirror_bridge_javascript.hpp"
#include "calculator.hpp"

MIRROR_BRIDGE_JS_MODULE(calculator,
    mirror_bridge::javascript::bind_class<Calculator>(env, m, "Calculator");
)
