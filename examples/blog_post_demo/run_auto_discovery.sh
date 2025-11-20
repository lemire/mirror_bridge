#!/bin/bash
# Automatically generate Python bindings from C++ using Mirror Bridge
set -e

echo "=========================================="
echo "  Mirror Bridge Auto-Discovery Demo"
echo "=========================================="
echo ""

# Check if we're in Docker
if ! command -v clang++ &> /dev/null || ! clang++ --version 2>&1 | grep -q "reflection\|p2996"; then
    echo "❌ Reflection-enabled compiler not found"
    echo ""
    echo "Please run inside the Mirror Bridge Docker container:"
    echo "  docker exec -it mirror_bridge_dev bash"
    echo "  cd /workspace/examples/blog_post_demo"
    echo "  ./run_auto_discovery.sh"
    exit 1
fi

echo "✓ Reflection compiler detected"
echo ""

# Step 1: Run auto-discovery (this also compiles!)
echo "[1/2] Running auto-discovery on C++ headers..."
echo ""

../../mirror_bridge_auto . --module image_processor

echo ""
echo "✓ Auto-discovery and compilation complete!"
echo ""

# Step 2: Copy the binding to current directory for easier import
echo "[2/2] Preparing binding for use..."
cp build/image_processor.so image_processor.so
echo "✓ Binding ready: image_processor.so"
echo ""

# Step 3: Quick test
echo "[3/3] Testing the binding..."
echo ""

LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH python3 << 'EOF'
import image_processor

# Quick smoke test
processor = image_processor.ImageProcessor(100, 100)
print(f"✓ Created ImageProcessor: {processor.get_width()}x{processor.get_height()}")

processor.fill_with_gradient()
print("✓ Filled with gradient")

processor.adjust_brightness(1.2)
print("✓ Applied brightness adjustment")

print("\n🎉 C++ binding is working!")
EOF

echo ""
echo "=========================================="
echo "  Success!"
echo "=========================================="
echo ""
echo "The C++ ImageProcessor is now available in Python!"
echo "Run the benchmark to see the speedup:"
echo "  python3 benchmark_comparison.py"
