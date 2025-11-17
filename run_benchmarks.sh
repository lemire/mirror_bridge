#!/bin/bash
# Wrapper script to run benchmarks in Docker

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "=========================================="
echo "  Mirror Bridge Benchmark Runner"
echo "=========================================="
echo ""
echo "Installing benchmark dependencies and running benchmarks..."
echo ""

# Use existing development image and install pybind11 + nanobind on-the-fly
docker run --rm -v "$SCRIPT_DIR:/workspace" mirror_bridge:full bash -c "
    apt-get update -qq &&
    apt-get install -y -qq pybind11-dev libboost-python-dev git cmake > /dev/null 2>&1 &&
    cd /tmp && git clone --depth 1 https://github.com/wjakob/nanobind.git > /dev/null 2>&1 &&
    cd nanobind && cmake -S . -B build && cmake --install build --prefix /usr/local > /dev/null 2>&1 &&
    cd /workspace &&
    ./benchmarks/run_all_benchmarks.sh
"

echo ""
echo "=========================================="
echo "  Benchmarks Complete!"
echo "=========================================="
echo ""
echo "Results saved to:"
echo "  - benchmarks/runtime/runtime_results.json"
echo ""
