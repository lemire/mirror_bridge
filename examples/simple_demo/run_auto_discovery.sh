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
    echo "  cd /workspace/examples/simple_demo"
    echo "  ./run_auto_discovery.sh"
    exit 1
fi

echo "✓ Reflection compiler detected"
echo ""

# Step 1: Run auto-discovery
echo "[1/3] Running auto-discovery on C++ headers..."
echo ""

../../mirror_bridge_auto . --module text_analyzer

echo ""
echo "✓ Auto-discovery complete!"
echo ""

# Step 2: Copy binding to current directory for easier import
echo "[2/3] Preparing binding..."
echo ""

cp build/text_analyzer.so text_analyzer.so

echo "✓ Binding ready: text_analyzer.so"
echo ""

# Step 3: Quick test
echo "[3/3] Testing the binding..."
echo ""

python3 << 'EOF'
import text_analyzer

# Quick smoke test
analyzer = text_analyzer.TextAnalyzer("Hello World!")
print(f"✓ Created TextAnalyzer")
print(f"  Characters: {analyzer.count_chars()}")
print(f"  Words: {analyzer.count_words()}")
print(f"  Vowels: {analyzer.count_vowels()}")
print(f"  Uppercase: {analyzer.to_uppercase()}")

print("\n🎉 C++ binding is working!")
EOF

echo ""
echo "=========================================="
echo "  Success!"
echo "=========================================="
echo ""
echo "The C++ TextAnalyzer is now available in Python!"
echo "Run the benchmark to see the speedup:"
echo "  python3 benchmark.py"
