#!/usr/bin/env python3
"""
Test nested class support in mirror_bridge.
Tests binding of classes that contain other bindable classes.
"""

import sys
import os

# Add build directory to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'build'))

try:
    import person
except ImportError as e:
    print(f"❌ Failed to import person: {e}")
    print("Make sure to build the bindings first with ../build_bindings.sh")
    sys.exit(1)

def test_person_nested():
    """Test Person class with nested Address class"""
    print("=== Mirror Bridge Nested Class Test ===\n")

    # Test 1: Create Person and check default values
    print("Test 1: Creating Person with default values...")
    p = person.Person()
    print(f"  ✓ Created Person")
    print(f"  name: '{p.name}'")
    print(f"  age: {p.age}")
    print(f"  address: {p.address}")
    assert p.name == "", "Default name should be empty"
    assert p.age == 0, "Default age should be 0"
    assert isinstance(p.address, dict), "Nested address should be a dict"
    print()

    # Test 2: Modify simple fields
    print("Test 2: Modifying simple fields...")
    p.name = "Alice"
    p.age = 30
    print(f"  Set name: '{p.name}'")
    print(f"  Set age: {p.age}")
    assert p.name == "Alice", "Name should be updated"
    assert p.age == 30, "Age should be updated"
    print(f"  ✓ Simple fields work correctly")
    print()

    # Test 3: Modify nested Address class
    print("Test 3: Modifying nested Address...")
    p.address = {
        "street": "123 Main St",
        "city": "Springfield",
        "zip_code": 12345
    }
    print(f"  Set address: {p.address}")
    assert p.address["street"] == "123 Main St", "Street should be updated"
    assert p.address["city"] == "Springfield", "City should be updated"
    assert p.address["zip_code"] == 12345, "Zip code should be updated"
    print(f"  ✓ Nested class works correctly")
    print()

    # Test 4: Partial address update
    print("Test 4: Partial address update...")
    new_address = {
        "street": "456 Oak Ave",
        "city": "Portland",
        "zip_code": 97201
    }
    p.address = new_address
    print(f"  Updated address: {p.address}")
    assert p.address["street"] == "456 Oak Ave", "Street should be updated"
    print(f"  ✓ Address can be completely replaced")
    print()

    # Test 5: Multiple instances with different addresses
    print("Test 5: Multiple independent instances...")
    p1 = person.Person()
    p2 = person.Person()
    p1.name = "Bob"
    p1.address = {"street": "1st St", "city": "NYC", "zip_code": 10001}
    p2.name = "Charlie"
    p2.address = {"street": "2nd St", "city": "LA", "zip_code": 90001}
    print(f"  p1: {p1.name} at {p1.address['city']}")
    print(f"  p2: {p2.name} at {p2.address['city']}")
    assert p1.address["city"] != p2.address["city"], "Instances should be independent"
    print(f"  ✓ Multiple instances work independently")
    print()

    print("========================================")
    print("✓ All nested class tests passed!")
    print("========================================")

if __name__ == "__main__":
    test_person_nested()
