# Mirror Bridge

Automatic C++ to Python bindings using C++26 reflection - write your class once, use it everywhere.

## What is Mirror Bridge?

Mirror Bridge generates zero-overhead Python bindings for C++ classes using compile-time reflection (P2996). No manual binding code required - reflection discovers your class structure automatically.

```cpp
// 1. Define your C++ class
struct Vector3 {
    double x, y, z;
};

// 2. Create binding (one line!)
MIRROR_BRIDGE_MODULE(vector3,
    mirror_bridge::bind_class<Vector3>(m, "Vector3");
)

// 3. Use from Python
import vector3
v = vector3.Vector3()
v.x = 3.0
```

## Key Features

- **🪞 Automatic Discovery**: Reflection finds all members - no manual specification
- **⚡ Zero Overhead**: All binding logic resolved at compile-time
- **📦 Single Header**: Just include `mirror_bridge.hpp` - that's it
- **🔄 Incremental Builds**: Hash-based change detection - only modified classes recompile
- **🎯 Type Safe**: Compiler enforces correctness via concepts
- **🐍 Python First**: Starting with Python, extensible to Rust and Lua

## Quick Start

### Using Docker (Recommended)

```bash
# Build Docker image with reflection compiler (30-60 min first time)
./docker_build.sh

# Run container
docker run -v $(pwd):/workspace -it mirror_bridge:latest

# Inside container: Run tests
cd /workspace/tests
../build_bindings.sh
python3 test_vector3.py
```

### With Local Compiler

Requires clang with P2996 reflection support (Bloomberg's clang-p2996).

```bash
export CXX=/path/to/clang++
cd tests
../build_bindings.sh
python3 test_vector3.py
```

## Project Structure

```
mirror_bridge/
├── mirror_bridge.hpp      # Single-header library
├── build_bindings.sh      # Incremental compilation script
├── Dockerfile             # Reflection compiler environment
├── docker_build.sh        # Docker setup helper
├── tests/                 # Examples and tests
│   ├── vector3.hpp        # Example C++ class
│   ├── vector3_binding.cpp # Binding declaration
│   └── test_vector3.py    # Python test
├── write_up.md            # Technical documentation
└── CLAUDE.md              # Development guidance
```

## How It Works

Mirror Bridge uses C++26 reflection to inspect classes at compile-time:

1. **Registration**: `bind_class<T>()` uses `std::meta::nonstatic_data_members_of(^^T)` to discover members
2. **Type Signature**: Reflection metadata generates a signature for change detection
3. **Binding Generation**: Python getters/setters are generated automatically for each member
4. **Incremental Compilation**: Script compares signatures, recompiles only changed classes

See [write_up.md](write_up.md) for detailed technical explanation.

## Requirements

- **Compiler**: clang with P2996 reflection support
  - Bloomberg's clang-p2996 (recommended)
  - Available at: https://github.com/bloomberg/clang-p2996
- **Python**: 3.8 or later
- **Build Tools**: CMake, Ninja (for building compiler)

## Incremental Compilation

The build script tracks class signatures and only recompiles when needed:

```bash
# First build - compiles everything
./build_bindings.sh

# Modify a class
echo "// Added comment" >> tests/vector3.hpp

# Rebuild - only vector3 recompiles
./build_bindings.sh
# Output: "Compiled: 1, Skipped: 1"
```

## Current Status

**⚠️ Experimental Implementation Note:**

This project demonstrates the architecture for reflection-based Python bindings. However, the Bloomberg clang-p2996 implementation we're using has incomplete `std::meta` namespace support. The code compiles successfully with `-freflection` but requires further work to match the P2996 proposal spec.

**Implemented:**
- ✅ Complete architecture design for reflection-based bindings
- ✅ Central registry with metadata tracking
- ✅ Hash-based incremental compilation system
- ✅ Concept-based type conversion framework
- ✅ Docker environment with clang-p2996 compiler
- ✅ Build infrastructure and test framework
- ✅ Comprehensive documentation

**Blocked (awaiting std::meta implementation):**
- ⏸️ Reflection-based member discovery (needs `std::meta::nonstatic_data_members_of`)
- ⏸️ Automatic getter/setter generation (needs `std::meta::identifier_of`)
- ⏸️ Runtime binding execution

**Future (requires reflection completion):**
- 🚧 Method binding (member functions)
- 🚧 Constructor parameter support
- 🚧 Container types (vector, map, etc.)
- 🚧 Inheritance handling
- 🚧 Rust and Lua backends

## Examples

### Vector3 (3D Vector)

```cpp
struct Vector3 {
    double x, y, z;
    double length() const;
    void normalize();
};
```

```python
v = vector3.Vector3()
v.x = 3.0
v.y = 4.0
v.z = 0.0
# Currently: only data members bound
# Coming soon: v.length() and v.normalize()
```

### Point2D (2D Point)

```cpp
struct Point2D {
    double x, y;
};
```

Demonstrates minimal class binding and incremental compilation.

## Philosophy

Mirror Bridge follows these principles:

1. **Single-header simplicity**: Minimal dependencies, easy integration
2. **Modern C++ only**: Use latest features for cleaner code
3. **Zero-overhead abstraction**: Compile-time work, no runtime cost
4. **Concept-driven design**: Generic, type-safe implementations
5. **Maintainability first**: Simple solutions over clever hacks

## Documentation

- **[write_up.md](write_up.md)**: Complete technical documentation
- **[tests/README.md](tests/README.md)**: Test examples and usage
- **[CLAUDE.md](CLAUDE.md)**: Project overview and architecture

## Contributing

This is an experimental project exploring C++26 reflection. Contributions welcome!

**Areas needing work:**
- Method binding via `std::meta::member_functions_of`
- Container support using concepts
- Constructor introspection
- Performance benchmarks vs. pybind11
- Rust/Lua backend implementations

## License

Apache License 2.0 - See [LICENSE](LICENSE) for details.

## Acknowledgments

- Bloomberg's clang-p2996 implementation of P2996 reflection
- simdjson project for concept-based design inspiration
- RBB project for exploring reflection-based bindings

---

**Status**: Experimental - C++26 reflection is not yet standardized.
