# Mirror Bridge - Automated Test Suite

This directory contains the automated test suite for Mirror Bridge, which builds and validates all Python bindings in sequence.

## Quick Start

**IMPORTANT**: Tests must run inside the Docker container (requires reflection-enabled compiler).

### Run All Tests (Automated)

```bash
# From your host machine, start/attach to the container:
./start_dev_container.sh

# Then inside the container:
cd /workspace
./tests/run_all_tests.sh
```

This script will:
1. Build all C++ bindings from `tests/e2e/*.cpp`
2. Run all Python tests from `tests/e2e/test_*.py`
3. Report comprehensive results with pass/fail status
4. Exit with code 0 if all tests pass, 1 if any fail

### Run Individual Tests

```bash
# Build a specific binding
cd /workspace/tests/e2e
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
  -I/workspace -fPIC -shared \
  $(python3-config --includes --ldflags) \
  vector3_binding.cpp -o ../../build/vector3.so

# Run a specific test
cd /workspace/build
LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH \
  python3 ../tests/e2e/test_vector3.py
```

## Test Structure

```
tests/
├── run_all_tests.sh          # Main automated test runner
├── README.md                  # This file
└── e2e/                       # End-to-end test cases
    ├── point2d.hpp            # C++ class definition
    ├── point2d_binding.cpp    # Mirror Bridge binding
    ├── test_point2d.py        # Python test
    ├── vector3.hpp            # Another test class
    ├── vector3_binding.cpp    # Its binding
    ├── test_vector3.py        # Its test
    └── ...                    # More test cases
```

## Current Test Status

| Test | Status | Description |
|------|--------|-------------|
| `test_point2d.py` | ✅ Pass | Basic 2D point with double members |
| `test_vector3.py` | ✅ Pass | 3D vector with arithmetic members |
| `test_particle.py` | ✅ Pass | Container support (std::vector, std::array) |
| `test_document.py` | ✅ Pass | String and vector<string> members |
| `test_person.py` | ✅ Pass | 2-level nesting (Person → Address) |
| `test_company.py` | ✅ Pass | 3-level nesting (Employee → Company → Address) |

## Features Tested

- ✅ **Basic member access**: Read/write primitive types (int, double, etc.)
- ✅ **String handling**: std::string members
- ✅ **Container support**: std::vector<T> and std::array<T, N>
- ✅ **Multiple instances**: Independent object lifetimes
- ✅ **Type safety**: Python<->C++ type conversions
- ✅ **Nested classes**: Arbitrary depth nesting via recursion (A → B → C → ...)
- ⏭️ **Methods**: Member function bindings (not yet implemented)

## Adding New Tests

1. **Create C++ header** in `tests/e2e/your_class.hpp`:
   ```cpp
   #pragma once
   #include <cmath>  // If using math functions

   struct YourClass {
       double x;
       double y;
       // ... add members
   };
   ```

2. **Create binding file** in `tests/e2e/your_class_binding.cpp`:
   ```cpp
   #include "mirror_bridge.hpp"
   #include "your_class.hpp"

   MIRROR_BRIDGE_MODULE(your_class,
       mirror_bridge::bind_class<YourClass>(m, "YourClass");
   )
   ```

3. **Create Python test** in `tests/e2e/test_your_class.py`:
   ```python
   #!/usr/bin/env python3
   import sys
   import os
   sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', '..', 'build'))

   import your_class

   def main():
       obj = your_class.YourClass()
       obj.x = 42.0
       assert obj.x == 42.0
       print("✓ Test passed")
       return 0

   if __name__ == "__main__":
       sys.exit(main())
   ```

4. **Run the test suite**:
   ```bash
   ./tests/run_all_tests.sh
   ```

The test runner automatically discovers and runs all tests matching the pattern `test_*.py`.

## Skipping Tests

To skip a test (e.g., due to unsupported features), edit `run_all_tests.sh`:

```bash
# Tests to skip (with reasons)
SKIP_TESTS=(
    "test_person.py:Nested classes not yet supported"
    "test_your_feature.py:Reason for skipping"
)
```

## Environment Requirements

The test runner automatically sets:
- `LD_LIBRARY_PATH`: Includes libc++ for reflection support
- `PYTHONPATH`: Includes build directory for module imports

Tests run in the `/workspace/tests/e2e` directory with access to `/workspace/build` for compiled bindings.

## Exit Codes

- `0`: All tests passed
- `1`: One or more tests failed or bindings failed to compile

## Verbose Mode

For detailed test output:

```bash
VERBOSE=1 ./tests/run_all_tests.sh
```

This shows full output from passing tests (normally only shown for failures).
