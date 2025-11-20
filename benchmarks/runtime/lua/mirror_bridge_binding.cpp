// Mirror Bridge Lua binding for runtime benchmarks
#include "lua/mirror_bridge_lua.hpp"
#include "../shared/benchmark_class.hpp"

MIRROR_BRIDGE_LUA_MODULE(bench_mb_lua,
    mirror_bridge::lua::bind_class<BenchmarkClass>(L, "BenchmarkClass");
)
