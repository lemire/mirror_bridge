#!/bin/bash
# Test mirror_bridge_generate (config file)

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
TEST_DIR="$(dirname "${BASH_SOURCE[0]}")"
BUILD_DIR="$PROJECT_ROOT/build"

echo "=== Testing mirror_bridge_generate ==="
echo ""

# Run config-file based generation
echo "Running: mirror_bridge_generate $TEST_DIR/vehicles.mirror"
cd "$PROJECT_ROOT"
./mirror_bridge_generate "$TEST_DIR/vehicles.mirror"

# Check if binding was created and compiled
if [ -f "$BUILD_DIR/vehicles_config.so" ]; then
    echo "✓ Config-file generation built vehicles_config.so"
else
    echo "✗ Failed to build vehicles_config.so"
    exit 1
fi

# Run Python test
echo ""
echo "Running Python validation..."
python3 << 'EOF'
import sys
sys.path.insert(0, 'build')

import vehicles_config

# Test Vehicle class
v = vehicles_config.Vehicle("Toyota", 2020, 15000.0)
assert v.brand == "Toyota"
assert v.year == 2020
assert v.mileage == 15000.0
assert v.age(2024) == 4

# Test Engine class
e = vehicles_config.Engine("V8", 450)
assert e.type == "V8"
assert e.horsepower == 450
assert e.is_powerful() == True

e2 = vehicles_config.Engine("I4", 200)
assert e2.is_powerful() == False

print("✓ Config-file generation test passed!")
EOF

echo ""
echo "✓ mirror_bridge_generate test completed successfully"
