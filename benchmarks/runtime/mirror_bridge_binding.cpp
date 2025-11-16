#include "mirror_bridge.hpp"
#include "benchmark_class.hpp"

MIRROR_BRIDGE_MODULE(bench_mb,
    mirror_bridge::bind_class<BenchmarkClass>(m, "BenchmarkClass");
)
