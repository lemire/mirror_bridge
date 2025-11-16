#!/bin/bash
# Mirror Bridge - Comprehensive Benchmark Suite
# Compares compile-time and runtime performance against pybind11 and Boost.Python

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BENCHMARK_DIR="$PROJECT_ROOT/benchmarks"
BUILD_DIR="$PROJECT_ROOT/build"

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}  Mirror Bridge - Benchmark Suite${NC}"
echo -e "${BLUE}=======================================${NC}"
echo ""

# Check for required tools
if ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: clang++ not found${NC}"
    echo "Run this script inside the Docker container"
    exit 1
fi

# Create build directory
mkdir -p "$BUILD_DIR"

# Function to measure compile time
measure_compile_time() {
    local binding_file=$1
    local output_file=$2
    local include_flags=$3

    # Clear cache
    sync
    echo 3 > /proc/sys/vm/drop_caches 2>/dev/null || true

    # Measure compilation time (3 runs, take median)
    local times=()
    for i in {1..3}; do
        rm -f "$output_file"
        start=$(date +%s%3N)
        clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
            $include_flags -fPIC -shared \
            $(python3-config --includes --ldflags) \
            "$binding_file" -o "$output_file" 2>&1 | grep -v "mixture of designated" > /dev/null || true
        end=$(date +%s%3N)
        times+=($((end - start)))
    done

    # Return median
    IFS=$'\n' sorted=($(sort -n <<<"${times[*]}"))
    echo "${sorted[1]}"
}

# Function to get file size in KB
get_size_kb() {
    stat -f%z "$1" 2>/dev/null | awk '{print int($1/1024)}' || stat -c%s "$1" 2>/dev/null | awk '{print int($1/1024)}'
}

echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  PART 1: Compile-Time Benchmarks${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""

# Simple benchmark
echo -e "${BLUE}Simple Project (1 class, ~10 methods)${NC}"
echo ""

cd "$BENCHMARK_DIR/compile_time/simple"

echo -n "  Mirror Bridge...  "
time_mb=$(measure_compile_time "mirror_bridge_binding.cpp" "$BUILD_DIR/simple_mb.so" "-I$PROJECT_ROOT")
size_mb=$(get_size_kb "$BUILD_DIR/simple_mb.so")
echo -e "${GREEN}${time_mb}ms  (${size_mb} KB)${NC}"

echo -n "  pybind11...       "
time_pb=$(measure_compile_time "pybind11_binding.cpp" "$BUILD_DIR/simple_pb.so" "-I/usr/include/python3.10")
size_pb=$(get_size_kb "$BUILD_DIR/simple_pb.so")
echo -e "${GREEN}${time_pb}ms  (${size_pb} KB)${NC}"

echo -n "  Boost.Python...   "
time_bp=$(measure_compile_time "boost_python_binding.cpp" "$BUILD_DIR/simple_bp.so" "-I/usr/include/python3.10")
size_bp=$(get_size_kb "$BUILD_DIR/simple_bp.so")
echo -e "${GREEN}${time_bp}ms  (${size_bp} KB)${NC}"

echo ""
ratio_mb_pb=$(awk "BEGIN {printf \"%.2f\", $time_mb/$time_pb}")
echo -e "  Speedup: Mirror Bridge vs pybind11:     ${ratio_mb_pb}x"
echo -e "  Speedup: Mirror Bridge vs Boost.Python: $(awk "BEGIN {printf \"%.2f\", $time_mb/$time_bp}")x"
echo ""

# Medium benchmark
echo -e "${BLUE}Medium Project (10 classes, ~50 methods)${NC}"
echo ""

cd "$BENCHMARK_DIR/compile_time/medium"

echo -n "  Mirror Bridge...  "
time_mb_med=$(measure_compile_time "mirror_bridge_binding.cpp" "$BUILD_DIR/medium_mb.so" "-I$PROJECT_ROOT")
size_mb_med=$(get_size_kb "$BUILD_DIR/medium_mb.so")
echo -e "${GREEN}${time_mb_med}ms  (${size_mb_med} KB)${NC}"

echo -n "  pybind11...       "
time_pb_med=$(measure_compile_time "pybind11_binding.cpp" "$BUILD_DIR/medium_pb.so" "-I/usr/include/python3.10")
size_pb_med=$(get_size_kb "$BUILD_DIR/medium_pb.so")
echo -e "${GREEN}${time_pb_med}ms  (${size_pb_med} KB)${NC}"

echo -n "  Boost.Python...   "
time_bp_med=$(measure_compile_time "boost_python_binding.cpp" "$BUILD_DIR/medium_bp.so" "-I/usr/include/python3.10")
size_bp_med=$(get_size_kb "$BUILD_DIR/medium_bp.so")
echo -e "${GREEN}${time_bp_med}ms  (${size_bp_med} KB)${NC}"

echo ""
ratio_mb_pb_med=$(awk "BEGIN {printf \"%.2f\", $time_mb_med/$time_pb_med}")
echo -e "  Speedup: Mirror Bridge vs pybind11:     ${ratio_mb_pb_med}x"
echo -e "  Speedup: Mirror Bridge vs Boost.Python: $(awk "BEGIN {printf \"%.2f\", $time_mb_med/$time_bp_med}")x"
echo ""

# Developer Experience metrics
echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  Developer Experience Metrics${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""

echo -e "${BLUE}Lines of Binding Code${NC}"
echo ""
echo "  Simple project:"
echo "    Mirror Bridge:  $(wc -l < "$BENCHMARK_DIR/compile_time/simple/mirror_bridge_binding.cpp" | tr -d ' ') lines"
echo "    pybind11:       $(wc -l < "$BENCHMARK_DIR/compile_time/simple/pybind11_binding.cpp" | tr -d ' ') lines"
echo "    Boost.Python:   $(wc -l < "$BENCHMARK_DIR/compile_time/simple/boost_python_binding.cpp" | tr -d ' ') lines"
echo ""
echo "  Medium project:"
echo "    Mirror Bridge:  $(wc -l < "$BENCHMARK_DIR/compile_time/medium/mirror_bridge_binding.cpp" | tr -d ' ') lines"
echo "    pybind11:       $(wc -l < "$BENCHMARK_DIR/compile_time/medium/pybind11_binding.cpp" | tr -d ' ') lines"
echo "    Boost.Python:   $(wc -l < "$BENCHMARK_DIR/compile_time/medium/boost_python_binding.cpp" | tr -d ' ') lines"
echo ""

# Runtime benchmarks
echo -e "${YELLOW}========================================${NC}"
echo -e "${YELLOW}  PART 2: Runtime Performance${NC}"
echo -e "${YELLOW}========================================${NC}"
echo ""

cd "$BENCHMARK_DIR/runtime"

# Build runtime benchmark modules
echo "Building runtime benchmark modules..."
echo ""

clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -I"$PROJECT_ROOT" -fPIC -shared \
    $(python3-config --includes --ldflags) \
    mirror_bridge_binding.cpp -o "$BUILD_DIR/bench_mb.so" 2>&1 | grep -v "mixture of designated" || true

clang++ -std=c++20 -stdlib=libc++ \
    -I/usr/include/python3.10 -fPIC -shared \
    $(python3-config --includes --ldflags) \
    pybind11_binding.cpp -o "$BUILD_DIR/bench_pb.so" 2>&1 | grep -v "mixture of designated" || true

clang++ -std=c++20 -stdlib=libc++ \
    -I/usr/include/python3.10 -fPIC -shared \
    $(python3-config --includes --ldflags) \
    -lboost_python310 \
    boost_python_binding.cpp -o "$BUILD_DIR/bench_bp.so" 2>&1 | grep -v "mixture of designated" || true

echo "Running runtime benchmarks (this may take a few minutes)..."
python3 run_runtime_benchmarks.py run

# Summary
echo ""
echo -e "${GREEN}========================================${NC}"
echo -e "${GREEN}  Benchmark Complete!${NC}"
echo -e "${GREEN}========================================${NC}"
echo ""
echo "Results summary:"
echo "  - Compile-time results: See above"
echo "  - Runtime results: See above table"
echo "  - Detailed runtime data: benchmarks/runtime/runtime_results.json"
echo ""
