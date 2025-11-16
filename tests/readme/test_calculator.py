#!/usr/bin/env python3
"""Test Calculator example from README introduction"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'build'))

import cpp_calc

print("=== README Calculator Example ===\n")

# Exact code from README
calc = cpp_calc.Calculator(100.0)  # Constructors with parameters ✓
calc.add(10)                       # Methods ✓
result = calc.subtract(3)          # Returns: 107.0
print(f"calc.value = {calc.value}")

# Verify results
assert calc.value == 107.0, f"Expected 107.0, got {calc.value}"
assert result == 107.0, f"subtract() should return 107.0, got {result}"

print("\n✓ README Calculator example works!")
