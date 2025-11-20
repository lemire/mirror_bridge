#!/bin/bash
# Generate flame graph visualization of Python performance bottlenecks
set -e

echo "=========================================="
echo "  Generating Flame Graph"
echo "=========================================="
echo ""

# Check if py-spy is available
if ! command -v py-spy &> /dev/null; then
    echo "Installing py-spy for flame graph generation..."
    pip3 install py-spy
fi

echo "[1/2] Running profiler on Python code..."
echo ""

# Run py-spy to generate flame graph
sudo py-spy record -o flamegraph_python.svg --duration 10 \
    -- python3 image_processor_pure_python.py

echo ""
echo "✓ Flame graph generated: flamegraph_python.svg"
echo ""

# Also generate using cProfile + gprof2dot if available
echo "[2/2] Generating call graph with cProfile..."
echo ""

python3 -m cProfile -o profile.stats image_processor_pure_python.py

if command -v gprof2dot &> /dev/null && command -v dot &> /dev/null; then
    gprof2dot -f pstats profile.stats | dot -Tpng -o callgraph.png
    echo "✓ Call graph generated: callgraph.png"
else
    echo "⚠ gprof2dot or graphviz not installed, skipping call graph"
    echo "  Install with: pip install gprof2dot && apt-get install graphviz"
fi

echo ""
echo "=========================================="
echo "  Visualization Complete!"
echo "=========================================="
echo ""
echo "View the flame graph:"
echo "  Open flamegraph_python.svg in your browser"
echo ""
echo "The flame graph shows:"
echo "  - Width = time spent in function"
echo "  - Height = call stack depth"
echo "  - Red/orange = hot spots (slow code)"
echo ""
