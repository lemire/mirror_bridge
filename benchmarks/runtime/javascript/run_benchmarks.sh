#!/bin/bash
# Runtime benchmarks for JavaScript bindings
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../../.." && pwd)"
BENCHMARK_DIR="$PROJECT_ROOT/benchmarks/runtime/javascript"
BUILD_DIR="$PROJECT_ROOT/build"

echo "======================================="
echo "  JavaScript Runtime Benchmarks"
echo "======================================="
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"

# Compile Mirror Bridge JavaScript binding
echo "[1/3] Compiling Mirror Bridge JavaScript binding..."
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT" \
    -I/usr/include/node \
    -fPIC -shared \
    "$BENCHMARK_DIR/mirror_bridge_binding.cpp" \
    -o "$BUILD_DIR/bench_mb_js.node"

echo "✓ Mirror Bridge JavaScript binding compiled"

# Compile Plain N-API binding
echo "[2/3] Compiling Plain N-API binding..."
clang++ -std=c++2c -stdlib=libc++ \
    -O3 -DNDEBUG \
    -I"$PROJECT_ROOT/benchmarks/runtime" \
    -I/usr/include/node \
    -fPIC -shared \
    "$BENCHMARK_DIR/plain_napi_binding.cpp" \
    -o "$BUILD_DIR/bench_plain_js.node"

echo "✓ Plain N-API binding compiled"

# Run benchmarks
echo "[3/3] Running benchmarks..."
echo ""
cd "$BENCHMARK_DIR"
LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH node run_runtime_benchmarks.js

echo ""
echo "JavaScript runtime benchmarks completed!"
