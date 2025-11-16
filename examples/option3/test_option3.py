#!/usr/bin/env python3
"""Test Option 3 - Config file approach"""

import sys
sys.path.insert(0, 'build')

import math_module

print("=== Option 3 Test: Config File Generation ===\n")

# Test Calculator
print("Test 1: Calculator methods")
calc = math_module.Calculator()
result = calc.compute(15, 5)  # (15 + 5) * 2 = 40
print(f"  calc.compute(15, 5) = {result}")
assert result == 40.0
print("  ✓ Calculator works\n")

# Test Vector3
print("Test 2: Vector3 methods")
v = math_module.Vector3()
v.x, v.y, v.z = 1.0, 2.0, 2.0
length = v.length()
print(f"  Vector({v.x}, {v.y}, {v.z}) length: {length}")
assert length == 3.0
v.normalize()
new_length = v.length()
print(f"  After normalize(), length: {new_length}")
assert abs(new_length - 1.0) < 0.0001
print("  ✓ Vector3 works\n")

print("=" * 40)
print("✓ Option 3 (Config File) Works!")
print("=" * 40)
