// Vec3 V8 Test Executable - Static Methods
// Embeds V8 and runs the JavaScript tests against Mirror Bridge bindings

#include "../v8_test_harness.hpp"

// Pull in the binding which defines vec3_init
#include "binding.cpp"

int main(int argc, char* argv[]) {
    const char* test_script = "test.js";
    if (argc > 1) {
        test_script = argv[1];
    }
    return run_v8_test(argc, argv, test_script, vec3_init);
}
