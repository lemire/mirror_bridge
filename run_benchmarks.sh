#!/bin/bash
# Helper script to build benchmark Docker image and run benchmarks

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
IMAGE_NAME="mirror_bridge:benchmarks"

echo "=========================================="
echo "  Mirror Bridge Benchmark Runner"
echo "=========================================="
echo ""

# Check if benchmark image exists
if ! docker images | grep -q "mirror_bridge.*benchmarks"; then
    echo "Benchmark Docker image not found. Building..."
    echo ""
    echo "This will take ~30-60 minutes (one-time setup)"
    echo "Building clang with reflection support..."
    echo ""

    docker build -f Dockerfile.benchmarks -t "$IMAGE_NAME" .

    echo ""
    echo "✓ Benchmark image built successfully"
    echo ""
else
    echo "✓ Benchmark image found"
    echo ""
fi

# Run benchmarks
echo "Running benchmarks inside Docker container..."
echo ""

docker run --rm -v "$SCRIPT_DIR:/workspace" "$IMAGE_NAME" bash -c "
    cd /workspace/benchmarks
    ./run_all_benchmarks.sh
"

echo ""
echo "=========================================="
echo "  Benchmarks Complete!"
echo "=========================================="
echo ""
echo "Results saved to:"
echo "  - benchmarks/runtime/runtime_results.json"
echo ""
