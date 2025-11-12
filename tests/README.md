# Mirror Bridge Tests

This directory contains end-to-end tests demonstrating the complete workflow of mirror_bridge.

## Test Structure

Each test consists of:
1. **C++ header file** (`.hpp`) - Defines the class to bind
2. **C++ binding file** (`.cpp`) - Registers the class with mirror_bridge
3. **Python test script** (`.py`) - Tests the generated binding

## Running Tests

### Inside Docker Container

```bash
# Build the Docker image (first time only)
./docker_build.sh

# Run the container
docker run -v $(pwd):/workspace -it mirror_bridge:latest

# Inside container: Build bindings
cd /workspace/tests
../build_bindings.sh

# Run Python tests
python3 test_vector3.py
```

### With Local Reflection Compiler

```bash
# Make sure you have clang with P2996 reflection support
export CXX=/path/to/clang++

# Build bindings
cd tests
../build_bindings.sh

# Run tests
python3 test_vector3.py
```

## Test Cases

### Vector3 Test
- **Class**: `Vector3` - 3D vector with x, y, z coordinates
- **Tests**: Object creation, member access, multiple instances
- **File**: `test_vector3.py`

### Point2D Test
- **Class**: `Point2D` - 2D point with x, y coordinates
- **Purpose**: Demonstrates incremental compilation (smaller class)
- **File**: `point2d_binding.cpp`

## Expected Output

```
=== Mirror Bridge End-to-End Test ===

Test 1: Creating Vector3 instance...
  ✓ Created Vector3: (0.0, 0.0, 0.0)
  ✓ Default values are correct

Test 2: Reading and writing members...
  Set values: x=3.0, y=4.0, z=0.0
  ✓ Member read/write works correctly

Test 3: Vector computations...
  Vector: (3.0, 4.0, 0.0)
  Expected length: 5.0
  ℹ Method binding not yet implemented (data members only)

Test 4: Multiple instances...
  v1.x = 1.0, v2.x = 2.0
  ✓ Multiple instances work independently

========================================
✓ All tests passed!
========================================
```

## Incremental Compilation Test

To test selective recompilation:

```bash
# Build all bindings
../build_bindings.sh

# Modify vector3.hpp (add a new member)
# Then rebuild
../build_bindings.sh

# Only vector3 will be recompiled
# point2d will be skipped (up-to-date)
```
