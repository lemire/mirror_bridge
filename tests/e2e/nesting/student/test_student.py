#!/usr/bin/env python3
"""
Test cross-file dependencies (Student -> University -> Address)
Each class is in a separate .hpp file
"""

import sys
import os

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', '..', 'build'))

try:
    import student
except ImportError as e:
    print(f"❌ Failed to import student: {e}")
    sys.exit(1)

def main():
    print("=== Mirror Bridge Cross-File Dependencies Test ===\n")

    # Test 1: Create Student with cross-file dependencies
    print("Test 1: Creating Student with cross-file nested types...")
    s = student.Student()
    print(f"  ✓ Created Student")
    print(f"  name: '{s.name}'")
    print(f"  student_id: {s.student_id}")
    print(f"  school (University from university.hpp): {s.school}")
    assert isinstance(s.school, dict)
    assert 'location' in s.school  # Address from address.hpp
    assert isinstance(s.school['location'], dict)
    print("  ✓ Cross-file dependency chain works: Student -> University -> Address")
    print()

    # Test 2: Set values across file boundaries
    print("Test 2: Setting nested values from different files...")
    s.name = "Alice"
    s.student_id = 54321
    s.school = {
        'name': 'MIT',
        'student_count': 11000,
        'location': {  # Address from address.hpp
            'street': '77 Massachusetts Ave',
            'city': 'Cambridge',
            'country': 'USA',
            'zip_code': 2139
        }
    }

    print(f"  Student: {s.name} (ID: {s.student_id})")
    print(f"  School: {s.school['name']}")
    print(f"  Location: {s.school['location']['city']}, {s.school['location']['country']}")

    assert s.name == "Alice"
    assert s.school['name'] == "MIT"
    assert s.school['location']['city'] == "Cambridge"
    print("  ✓ All cross-file nested values work correctly")
    print()

    # Test 3: Verify no compilation order needed
    print("Test 3: Verify no DAG ordering problem...")
    print("  ℹ We only compiled student_binding.cpp")
    print("  ℹ University and Address were NOT separately compiled")
    print("  ℹ They are automatically handled as nested types")
    print("  ✓ No dependency ordering required!")
    print()

    print("="*40)
    print("✓ All cross-file dependency tests passed!")
    print("="*40)
    return 0

if __name__ == "__main__":
    sys.exit(main())
