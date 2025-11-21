#!/bin/bash
# Generate Python bindings for the C++ PatternMatcher
set -e

echo "=========================================="
echo "  Mirror Bridge Auto-Discovery"
echo "=========================================="
echo ""

# Check if we're in Docker
if ! command -v clang++ &> /dev/null || ! clang++ --version 2>&1 | grep -q "reflection\|p2996"; then
    echo "❌ Reflection-enabled compiler not found"
    echo ""
    echo "Please run inside the Mirror Bridge Docker container:"
    echo "  docker exec -it mirror_bridge_dev bash"
    echo "  cd /workspace/examples/hackernews_demo"
    echo "  ./run_auto_discovery.sh"
    exit 1
fi

echo "✓ Reflection compiler detected"
echo ""

echo "[1/2] Auto-discovering C++ classes and generating bindings..."
echo ""

../../mirror_bridge_auto . --module pattern_matcher

echo ""
echo "✓ Auto-discovery and compilation complete!"
echo ""

echo "[2/2] Copying binding for easy import..."
cp build/pattern_matcher.so pattern_matcher.so
echo "✓ Binding ready: pattern_matcher.so"
echo ""

echo "[3/3] Testing the binding..."
echo ""

LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH python3 << 'EOF'
import pattern_matcher

# Quick test
matcher = pattern_matcher.PatternMatcher()
text = "Hello ERROR world ERROR test ERROR"
count = matcher.find_pattern(text, "ERROR")
print(f"✓ Found {count} matches in test string")

print("\n🎉 C++ PatternMatcher is working!")
EOF

echo ""
echo "=========================================="
echo "  Success!"
echo "=========================================="
echo ""
echo "The C++ PatternMatcher is ready!"
echo "Run the benchmark:"
echo "  ./run_benchmark.sh"
