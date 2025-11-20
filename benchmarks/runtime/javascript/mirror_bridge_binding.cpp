// Mirror Bridge JavaScript binding for runtime benchmarks
#include "javascript/mirror_bridge_javascript.hpp"
#include "../shared/benchmark_class.hpp"

MIRROR_BRIDGE_JS_MODULE(bench_mb_js,
    mirror_bridge::javascript::bind_class<BenchmarkClass>(env, m, "BenchmarkClass");
)
