#!/usr/bin/env python3
"""
Test deep nesting support (3 levels: Employee -> Company -> Address)
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'build'))

try:
    import company
except ImportError as e:
    print(f"❌ Failed to import company: {e}")
    sys.exit(1)

def main():
    print("=== Mirror Bridge Deep Nesting Test ===\n")

    # Test 1: Create Employee with default values
    print("Test 1: Creating Employee with 3-level nesting...")
    emp = company.Employee()
    print(f"  ✓ Created Employee")
    print(f"  name: '{emp.name}'")
    print(f"  id: {emp.id}")
    print(f"  employer (Company): {emp.employer}")
    assert emp.name == ""
    assert emp.id == 0
    assert isinstance(emp.employer, dict)
    assert 'headquarters' in emp.employer
    assert isinstance(emp.employer['headquarters'], dict)
    print("  ✓ 3-level nesting structure verified")
    print()

    # Test 2: Set deeply nested values
    print("Test 2: Setting deeply nested address...")
    emp.name = "Bob"
    emp.id = 12345
    emp.employer = {
        'name': 'TechCorp',
        'employee_count': 500,
        'headquarters': {
            'street': '100 Tech Blvd',
            'city': 'San Francisco',
            'zip_code': 94105
        }
    }
    print(f"  Set employee: {emp.name}")
    print(f"  Set company: {emp.employer['name']}")
    print(f"  Set HQ city: {emp.employer['headquarters']['city']}")

    # Verify all 3 levels
    assert emp.name == "Bob"
    assert emp.employer['name'] == "TechCorp"
    assert emp.employer['headquarters']['city'] == "San Francisco"
    print("  ✓ All 3 levels accessible")
    print()

    # Test 3: Partial update of nested structure
    print("Test 3: Updating middle level (Company)...")
    emp.employer = {
        'name': 'NewCorp',
        'employee_count': 1000,
        'headquarters': {
            'street': '200 New St',
            'city': 'Seattle',
            'zip_code': 98101
        }
    }
    assert emp.employer['name'] == "NewCorp"
    assert emp.employer['headquarters']['city'] == "Seattle"
    print(f"  ✓ Updated to {emp.employer['name']} in {emp.employer['headquarters']['city']}")
    print()

    print("="*40)
    print("✓ All deep nesting tests passed!")
    print("="*40)
    return 0

if __name__ == "__main__":
    sys.exit(main())
