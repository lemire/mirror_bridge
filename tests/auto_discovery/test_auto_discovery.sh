#!/bin/bash
# Test mirror_bridge_auto (auto-discovery)

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TEST_DIR="$(dirname "${BASH_SOURCE[0]}")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== Testing mirror_bridge_auto ==="
echo ""

# Run auto-discovery on this directory
echo "Running: mirror_bridge_auto $TEST_DIR --module geometry_auto"
cd "$PROJECT_ROOT"
./mirror_bridge_auto "$TEST_DIR" --module geometry_auto

# Check if binding was created and compiled
if [ -f "$BUILD_DIR/geometry_auto.so" ]; then
    echo "✓ Auto-discovery generated and built geometry_auto.so"
else
    echo "✗ Failed to build geometry_auto.so"
    exit 1
fi

# Run Python test
echo ""
echo "Running Python validation..."
python3 << 'EOF'
import sys
sys.path.insert(0, 'build')

import geometry_auto

# Test Point class
p = geometry_auto.Point(3.0, 4.0)
assert p.x == 3.0
assert p.y == 4.0
assert abs(p.distance_from_origin() - 5.0) < 0.001

# Test Circle class
c = geometry_auto.Circle(0, 0, 10)
assert c.radius == 10.0
assert abs(c.area() - 314.159) < 0.01

print("✓ Auto-discovery test passed!")
EOF

echo ""
echo "✓ mirror_bridge_auto test completed successfully"
