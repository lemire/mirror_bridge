#!/bin/bash
# Runtime benchmarks for Lua bindings
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
BENCHMARK_DIR="$PROJECT_ROOT/benchmarks/runtime/lua"
BUILD_DIR="$PROJECT_ROOT/build"

echo "======================================="
echo "  Lua Runtime Benchmarks"
echo "======================================="
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"

# Compile Mirror Bridge Lua binding
echo "[1/5] Compiling Mirror Bridge Lua binding..."
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT" \
    -I/usr/include/lua5.4 \
    -fPIC -shared \
    "$BENCHMARK_DIR/mirror_bridge_binding.cpp" \
    -o "$BUILD_DIR/bench_mb_lua.so" \
    -llua5.4

echo "✓ Mirror Bridge Lua binding compiled"

# Compile Plain Lua C API binding
echo "[2/5] Compiling Plain Lua C API binding..."
clang++ -std=c++2c -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT/benchmarks/runtime" \
    -I/usr/include/lua5.4 \
    -fPIC -shared \
    "$BENCHMARK_DIR/plain_lua_binding.cpp" \
    -o "$BUILD_DIR/bench_plain_lua.so" \
    -llua5.4

echo "✓ Plain Lua C API binding compiled"

# Compile sol2 binding
echo "[3/5] Compiling sol2 binding..."
clang++ -std=c++17 -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT/benchmarks/runtime" \
    -I/tmp/sol2/include \
    -I/usr/include/lua5.4 \
    -fPIC -shared \
    "$BENCHMARK_DIR/sol2_binding.cpp" \
    -o "$BUILD_DIR/bench_sol2.so" \
    -llua5.4 2>&1 | grep -v "ignoring return value" || true

echo "✓ sol2 binding compiled"

# Compile LuaBridge3 binding
echo "[4/5] Compiling LuaBridge3 binding..."
clang++ -std=c++17 -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT/benchmarks/runtime" \
    -I/tmp/LuaBridge3/Source \
    -I/usr/include/lua5.4 \
    -fPIC -shared \
    "$BENCHMARK_DIR/luabridge_binding.cpp" \
    -o "$BUILD_DIR/bench_luabridge.so" \
    -llua5.4 2>&1 | grep -v "ignoring return value" || true

echo "✓ LuaBridge3 binding compiled"

# Run benchmarks
echo "[5/5] Running benchmarks..."
echo ""
cd "$BENCHMARK_DIR"
LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH lua5.4 run_runtime_benchmarks.lua

echo ""
echo "Lua runtime benchmarks completed!"
