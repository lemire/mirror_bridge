#!/usr/bin/env python3
"""
Test Vec3 bindings for:
1. Static methods (hot_loop, add_static)
2. Methods with const reference parameters (dot)
"""

import sys
import os

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import vec3_test
except ImportError as e:
    print(f"ERROR: Failed to import vec3_test: {e}")
    print("Build it first with: ./build_vec3_test.sh")
    sys.exit(1)


def test_basic_construction():
    """Test basic Vec3 construction"""
    print("Testing basic construction...")
    v = vec3_test.Vec3(1.0, 2.0, 3.0)
    assert abs(v.x - 1.0) < 1e-10, f"Expected x=1.0, got {v.x}"
    assert abs(v.y - 2.0) < 1e-10, f"Expected y=2.0, got {v.y}"
    assert abs(v.z - 3.0) < 1e-10, f"Expected z=3.0, got {v.z}"
    print("  PASS: Basic construction works")


def test_const_ref_parameter():
    """Test method with const reference parameter (dot)"""
    print("Testing const reference parameter (dot method)...")
    a = vec3_test.Vec3(1.0, 2.0, 3.0)
    b = vec3_test.Vec3(4.0, 5.0, 6.0)

    # dot product: 1*4 + 2*5 + 3*6 = 4 + 10 + 18 = 32
    result = a.dot(b)
    expected = 32.0

    assert abs(result - expected) < 1e-10, f"Expected dot={expected}, got {result}"
    print(f"  PASS: a.dot(b) = {result}")


def test_length_method():
    """Test length method"""
    print("Testing length method...")
    v = vec3_test.Vec3(3.0, 4.0, 0.0)
    result = v.length()
    expected = 5.0  # 3-4-5 triangle

    assert abs(result - expected) < 1e-10, f"Expected length={expected}, got {result}"
    print(f"  PASS: length() = {result}")


def test_static_method_hot_loop():
    """Test static method hot_loop"""
    print("Testing static method (hot_loop)...")

    # Check if hot_loop exists as a static method
    if not hasattr(vec3_test.Vec3, 'hot_loop'):
        print("  FAIL: hot_loop static method not found!")
        print(f"  Available attributes: {[a for a in dir(vec3_test.Vec3) if not a.startswith('_')]}")
        return False

    # Call the static method
    result = vec3_test.Vec3.hot_loop(1000)

    # Verify result is reasonable (should be a positive number)
    assert result > 0, f"Expected positive result, got {result}"
    print(f"  PASS: Vec3.hot_loop(1000) = {result}")
    return True


def test_static_method_add_static():
    """Test static method add_static with const ref parameters"""
    print("Testing static method with const ref params (add_static)...")

    if not hasattr(vec3_test.Vec3, 'add_static'):
        print("  FAIL: add_static static method not found!")
        return False

    a = vec3_test.Vec3(1.0, 2.0, 3.0)
    b = vec3_test.Vec3(4.0, 5.0, 6.0)

    # Call static method with Vec3 references
    result = vec3_test.Vec3.add_static(a, b)

    assert abs(result.x - 5.0) < 1e-10, f"Expected x=5.0, got {result.x}"
    assert abs(result.y - 7.0) < 1e-10, f"Expected y=7.0, got {result.y}"
    assert abs(result.z - 9.0) < 1e-10, f"Expected z=9.0, got {result.z}"
    print(f"  PASS: Vec3.add_static(a, b) = ({result.x}, {result.y}, {result.z})")
    return True


def main():
    print("=" * 60)
    print("Vec3 Binding Tests")
    print("=" * 60)
    print()

    all_passed = True

    try:
        test_basic_construction()
    except Exception as e:
        print(f"  FAIL: {e}")
        all_passed = False

    try:
        test_const_ref_parameter()
    except Exception as e:
        print(f"  FAIL: {e}")
        all_passed = False

    try:
        test_length_method()
    except Exception as e:
        print(f"  FAIL: {e}")
        all_passed = False

    try:
        if not test_static_method_hot_loop():
            all_passed = False
    except Exception as e:
        print(f"  FAIL: {e}")
        all_passed = False

    try:
        if not test_static_method_add_static():
            all_passed = False
    except Exception as e:
        print(f"  FAIL: {e}")
        all_passed = False

    print()
    print("=" * 60)
    if all_passed:
        print("ALL TESTS PASSED!")
        return 0
    else:
        print("SOME TESTS FAILED!")
        return 1


if __name__ == "__main__":
    sys.exit(main())
