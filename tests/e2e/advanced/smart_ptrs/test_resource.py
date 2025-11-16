#!/usr/bin/env python3
"""Test smart pointer support (unique_ptr, shared_ptr)"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', 'build'))

import resource

print("=== Smart Pointer Support Test ===\n")

rm = resource.ResourceManager()

# Test smart pointer data members
print("Test 1: Smart pointer data members")
print(f"  Initial unique_data: {rm.unique_data}")
print(f"  Initial shared_data: {rm.shared_data}")
# Smart pointers should convert to None when null
assert rm.unique_data is None
assert rm.shared_data is None
print("  ✓ Null smart pointers convert to None\n")

# Test methods returning smart pointers
print("Test 2: Methods returning smart pointers")

# These return smart pointers, which should be converted to dicts
unique_result = rm.create_unique("test1", 42)
print(f"  create_unique('test1', 42) = {unique_result}")
# Should be dict representation of Data
assert isinstance(unique_result, dict)
assert unique_result['name'] == "test1"
assert unique_result['value'] == 42
print("  ✓ unique_ptr return converted to dict\n")

shared_result = rm.create_shared("test2", 99)
print(f"  create_shared('test2', 99) = {shared_result}")
assert isinstance(shared_result, dict)
assert shared_result['name'] == "test2"
assert shared_result['value'] == 99
print("  ✓ shared_ptr return converted to dict\n")

# Test setting smart pointer members
print("Test 3: Setting smart pointer members from Python")

# Set smart pointer from dict
data_dict = {'name': 'from_python', 'value': 123}
rm.unique_data = data_dict
print(f"  Set unique_data = {data_dict}")
print(f"  get_unique_name() = '{rm.get_unique_name()}'")
print(f"  get_unique_value() = {rm.get_unique_value()}")
assert rm.get_unique_name() == "from_python"
assert rm.get_unique_value() == 123
print("  ✓ Dict → unique_ptr conversion works\n")

# Set shared_ptr
rm.shared_data = {'name': 'shared', 'value': 456}
print(f"  Set shared_data = {{'name': 'shared', 'value': 456}}")
assert rm.get_shared_name() == "shared"
assert rm.get_shared_value() == 456
print("  ✓ Dict → shared_ptr conversion works\n")

# Test setting to None
print("Test 4: Setting smart pointers to None")
rm.unique_data = None
rm.shared_data = None
assert rm.get_unique_name() == "null"
assert rm.get_shared_name() == "null"
print("  ✓ None → null smart pointer works\n")

# Test methods taking smart pointers
print("Test 5: Methods taking smart pointers")
# Methods that take smart pointers should accept dicts
rm.set_unique({'name': 'method_test', 'value': 789})
assert rm.get_unique_name() == "method_test"
assert rm.get_unique_value() == 789
print("  ✓ Method with unique_ptr parameter works\n")

rm.set_shared({'name': 'shared_method', 'value': 321})
assert rm.get_shared_name() == "shared_method"
assert rm.get_shared_value() == 321
print("  ✓ Method with shared_ptr parameter works\n")

print("="*40)
print("✓ All smart pointer tests passed!")
print("  - unique_ptr and shared_ptr fully supported")
print("  - Automatic conversion to/from Python dicts")
print("  - None handling for null pointers")
print("="*40)
