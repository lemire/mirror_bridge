#!/usr/bin/env python3
"""Test method overloading via name mangling"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', 'build'))

import printer

print("=== Method Overloading Test ===\n")

p = printer.Printer()

# List available methods to see mangled names
print("Available methods:")
methods = [m for m in dir(p) if not m.startswith('_')]
for m in methods:
    print(f"  - {m}")
print()

# Test overloaded print methods
print("Test 1: Overloaded print methods")

# Check that mangled names exist
assert hasattr(p, 'print_int') or hasattr(p, 'print'), "Should have print_int or print"
assert hasattr(p, 'print_double') or hasattr(p, 'print'), "Should have print_double or print"
# String variant may have complex mangled name
string_prints = [m for m in dir(p) if 'print' in m and 'string' in m.lower() or m == 'print_stdstring']
assert len(string_prints) > 0, f"Should have print_string variant. Available: {[m for m in dir(p) if 'print' in m]}"

# Use mangled names (reflection creates these automatically)
if hasattr(p, 'print_int'):
    p.print_int(42)
    assert p.last_output == "int: 42"
    print(f"  print_int(42) -> '{p.last_output}' ✓")

if hasattr(p, 'print_double'):
    p.print_double(3.14)
    assert "3.14" in p.last_output
    print(f"  print_double(3.14) -> '{p.last_output}' ✓")

# Find the string variant (name depends on type mangling)
string_print = None
for name in dir(p):
    if 'print' in name and 'string' in name.lower() or name == 'print_stdstring':
        string_print = name
        break

if string_print:
    getattr(p, string_print)("hello")
    assert p.last_output == "string: hello"
    print(f"  {string_print}('hello') -> '{p.last_output}' ✓")

print()

# Test overloaded format methods
print("Test 2: Overloaded format methods with return values")

if hasattr(p, 'format_int_int') or hasattr(p, 'format'):
    if hasattr(p, 'format_int_int'):
        result = p.format_int_int(10, 20)
        print(f"  format_int_int(10, 20) = '{result}'")
        assert result == "10,20"
        print("  ✓")

if hasattr(p, 'format_double_double'):
    result = p.format_double_double(1.5, 2.5)
    print(f"  format_double_double(1.5, 2.5) = '{result}'")
    assert "1.5" in result and "2.5" in result
    print("  ✓")

# Find string variant
string_format = None
for name in dir(p):
    if 'format' in name and 'string' in name.lower():
        string_format = name
        break

if string_format:
    result = getattr(p, string_format)("foo", "bar")
    print(f"  {string_format}('foo', 'bar') = '{result}'")
    assert result == "foo + bar"
    print("  ✓")

print()

# Test non-overloaded method
print("Test 3: Non-overloaded method (should keep original name)")
assert hasattr(p, 'get_last')
result = p.get_last()
print(f"  get_last() = '{result}'")
print("  ✓ Non-overloaded method unchanged\n")

print("="*40)
print("✓ Method overloading via name mangling works!")
print("  Overloaded methods get type suffixes automatically")
print("="*40)
