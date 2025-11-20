#!/bin/bash
# Compile-time benchmarks for Lua and JavaScript bindings
set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
BENCHMARK_DIR="$PROJECT_ROOT/benchmarks/compile_time"
BUILD_DIR="$PROJECT_ROOT/build"

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Lua/JS Compile-Time Benchmarks${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Create build directory
mkdir -p "$BUILD_DIR"

# Function to measure Lua compile time
measure_lua_compile_time() {
    local binding_file=$1
    local output_file=$2
    local include_dir=$3

    # Clear cache
    sync
    echo 3 > /proc/sys/vm/drop_caches 2>/dev/null || true

    # Measure compilation time (5 runs)
    local times=()
    for i in {1..5}; do
        rm -f "$output_file"
        start=$(date +%s%3N)
        clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
            -O3 -DNDEBUG \
            -I"$PROJECT_ROOT" \
            -I"$include_dir" \
            -I/usr/include/lua5.4 \
            -fPIC -shared \
            "$binding_file" -o "$output_file" \
            -llua5.4 2>&1 | grep -v "mixture of designated\|does not define __cpp_reflection\|deprecated" > /dev/null || true
        end=$(date +%s%3N)
        times+=($((end - start)))
    done

    # Calculate median and stddev
    IFS=$'\n' sorted=($(sort -n <<<"${times[*]}"))
    local median="${sorted[2]}"

    # Calculate mean for stddev
    local sum=0
    for t in "${times[@]}"; do
        sum=$((sum + t))
    done
    local mean=$((sum / 5))

    # Calculate standard deviation
    local sq_sum=0
    for t in "${times[@]}"; do
        local diff=$((t - mean))
        sq_sum=$((sq_sum + diff * diff))
    done
    local variance=$((sq_sum / 5))
    local stddev=$(awk "BEGIN {printf \"%.0f\", sqrt($variance)}")

    echo "${median} ${stddev}"
}

# Function to measure JavaScript compile time
measure_js_compile_time() {
    local binding_file=$1
    local output_file=$2
    local include_dir=$3

    # Clear cache
    sync
    echo 3 > /proc/sys/vm/drop_caches 2>/dev/null || true

    # Measure compilation time (5 runs)
    local times=()
    for i in {1..5}; do
        rm -f "$output_file"
        start=$(date +%s%3N)
        clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
            -O3 -DNDEBUG \
            -I"$PROJECT_ROOT" \
            -I"$include_dir" \
            -I/usr/include/node \
            -fPIC -shared \
            "$binding_file" -o "$output_file" 2>&1 | grep -v "mixture of designated\|does not define __cpp_reflection\|deprecated" > /dev/null || true
        end=$(date +%s%3N)
        times+=($((end - start)))
    done

    # Calculate median and stddev
    IFS=$'\n' sorted=($(sort -n <<<"${times[*]}"))
    local median="${sorted[2]}"

    # Calculate mean for stddev
    local sum=0
    for t in "${times[@]}"; do
        sum=$((sum + t))
    done
    local mean=$((sum / 5))

    # Calculate standard deviation
    local sq_sum=0
    for t in "${times[@]}"; do
        local diff=$((t - mean))
        sq_sum=$((sq_sum + diff * diff))
    done
    local variance=$((sq_sum / 5))
    local stddev=$(awk "BEGIN {printf \"%.0f\", sqrt($variance)}")

    echo "${median} ${stddev}"
}

# Format time
format_time() {
    local ms=$1
    if [ $ms -lt 1000 ]; then
        echo "${ms}ms"
    else
        echo "$(awk "BEGIN {printf \"%.2f\", $ms/1000}")s"
    fi
}

echo -e "${GREEN}[1/4] Simple Class - Lua Binding${NC}"
result_simple_lua=$(measure_lua_compile_time \
    "$BENCHMARK_DIR/simple/mirror_bridge_binding_lua.cpp" \
    "$BUILD_DIR/simple_mb_lua.so" \
    "$BENCHMARK_DIR/simple")
median_simple_lua=$(echo $result_simple_lua | cut -d' ' -f1)
stddev_simple_lua=$(echo $result_simple_lua | cut -d' ' -f2)
echo -e "  Compile time: $(format_time $median_simple_lua) ± $(format_time $stddev_simple_lua)"

echo -e "${GREEN}[2/4] Simple Class - JavaScript Binding${NC}"
result_simple_js=$(measure_js_compile_time \
    "$BENCHMARK_DIR/simple/mirror_bridge_binding_js.cpp" \
    "$BUILD_DIR/simple_mb_js.node" \
    "$BENCHMARK_DIR/simple")
median_simple_js=$(echo $result_simple_js | cut -d' ' -f1)
stddev_simple_js=$(echo $result_simple_js | cut -d' ' -f2)
echo -e "  Compile time: $(format_time $median_simple_js) ± $(format_time $stddev_simple_js)"

echo -e "${GREEN}[3/4] Medium Class - Lua Binding${NC}"
result_medium_lua=$(measure_lua_compile_time \
    "$BENCHMARK_DIR/medium/mirror_bridge_binding_lua.cpp" \
    "$BUILD_DIR/medium_mb_lua.so" \
    "$BENCHMARK_DIR/medium")
median_medium_lua=$(echo $result_medium_lua | cut -d' ' -f1)
stddev_medium_lua=$(echo $result_medium_lua | cut -d' ' -f2)
echo -e "  Compile time: $(format_time $median_medium_lua) ± $(format_time $stddev_medium_lua)"

echo -e "${GREEN}[4/4] Medium Class - JavaScript Binding${NC}"
result_medium_js=$(measure_js_compile_time \
    "$BENCHMARK_DIR/medium/mirror_bridge_binding_js.cpp" \
    "$BUILD_DIR/medium_mb_js.node" \
    "$BENCHMARK_DIR/medium")
median_medium_js=$(echo $result_medium_js | cut -d' ' -f1)
stddev_medium_js=$(echo $result_medium_js | cut -d' ' -f2)
echo -e "  Compile time: $(format_time $median_medium_js) ± $(format_time $stddev_medium_js)"

# Print summary table
echo ""
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}  Compile-Time Summary${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
printf "%-20s %-20s %-20s\n" "Benchmark" "Lua" "JavaScript"
echo "------------------------------------------------------------"
printf "%-20s %-20s %-20s\n" "Simple Class" "$(format_time $median_simple_lua)" "$(format_time $median_simple_js)"
printf "%-20s %-20s %-20s\n" "Medium Class" "$(format_time $median_medium_lua)" "$(format_time $median_medium_js)"
echo ""
echo -e "${GREEN}Compile-time benchmarks completed!${NC}"
