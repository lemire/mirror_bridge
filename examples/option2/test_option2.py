#!/usr/bin/env python3
"""Test Option 2 - Auto-Discovery"""

import sys
sys.path.insert(0, 'build')

import math_module

print("=== Option 2 Test: Auto-Discovery ===\n")

# Test Calculator (auto-discovered)
print("Test 1: Auto-discovered Calculator")
calc = math_module.Calculator()
calc.add(15)
calc.subtract(5)
print(f"  calc value: {calc.value}")
assert calc.value == 10.0
print("  ✓ Calculator works\n")

# Test Vector3 (auto-discovered)
print("Test 2: Auto-discovered Vector3")
v = math_module.Vector3()
v.x, v.y, v.z = 6.0, 8.0, 0.0
length = v.length()
print(f"  Vector({v.x}, {v.y}, {v.z}) length: {length}")
assert length == 10.0
print("  ✓ Vector3 works\n")

print("=" * 40)
print("✓ Option 2 (Auto-Discovery) Works!")
print("  All classes in src/ were automatically bound!")
print("=" * 40)
