# Mirror Bridge: Zero-Overhead C++ to Python Bindings via Reflection

## Overview

Mirror Bridge is a single-header library that automatically generates Python bindings for C++ classes using C++26 reflection (P2996). Unlike traditional binding libraries like pybind11 or Boost.Python, mirror_bridge eliminates manual binding code through compile-time introspection, achieving true zero-overhead abstractions.

## Core Architecture

### 1. Reflection-Based Type Discovery

The foundation of mirror_bridge is C++26's reflection capability, specifically the P2996 proposal implemented in Bloomberg's clang fork. Reflection allows compile-time inspection of class structures:

```cpp
template<typename T>
consteval std::string generate_type_signature() {
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    // Iterate over members and extract their names and types
    // This happens entirely at compile-time
}
```

**Key Benefits:**
- No runtime overhead - all introspection happens during compilation
- Automatic discovery of class members without manual specification
- Type-safe by construction - compiler enforces correctness

### 2. Central Registry System

Classes are registered in a global registry that tracks their metadata for incremental compilation:

```cpp
class Registry {
    std::unordered_map<std::string, ClassMetadata> classes_;

    // Each ClassMetadata contains:
    // - Type signature (reflection-derived)
    // - Hash of the signature
    // - Python type object (after binding generation)
};
```

**Hash-Based Change Detection:**
- Class signatures are computed from reflection metadata
- Changes to member variables trigger recompilation
- Unchanged classes are skipped during rebuild
- This enables selective, incremental builds for large codebases

### 3. Type Conversion Layer

The library uses C++20 concepts to handle type conversions generically:

```cpp
template<Arithmetic T>
PyObject* to_python(const T& value);

template<StringLike T>
PyObject* to_python(const T& value);

template<Bindable T>
PyTypeObject* bind_class(PyObject* module, const char* name);
```

**Concept-Based Design:**
- `Arithmetic` concept: Handles int, float, double, etc.
- `StringLike` concept: Handles std::string, const char*, std::string_view
- `Bindable` concept: Identifies classes that can be bound (has reflection metadata)

This approach is similar to how simdjson uses concepts for JSON serialization - letting the compiler select the correct implementation at compile-time.

### 4. Python C API Integration

Each bound class is wrapped in a Python object structure:

```cpp
template<typename T>
struct PyWrapper {
    PyObject_HEAD
    T* cpp_object;  // Pointer to actual C++ instance
    bool owns;      // Ownership flag for cleanup
};
```

**Automatic Getter/Setter Generation:**
- Reflection provides member count and types
- Getters/setters are generated at compile-time using index sequences
- Each member is directly accessible from Python with no indirection

### 5. Single-Header Philosophy

The entire implementation is contained in `mirror_bridge.hpp`:
- No build system complexity
- Easy integration - just `#include "mirror_bridge.hpp"`
- No runtime dependencies beyond Python C API
- Minimal bloat - concepts enable generic code without template explosion

## Build System: Incremental Compilation

The `build_bindings.sh` script implements selective recompilation:

```bash
# For each binding file:
1. Extract class names from bind_class<> calls
2. Compute SHA256 hash of class definition
3. Compare with stored hash in database
4. If changed or missing: compile the binding
5. If unchanged: skip compilation
6. Update hash database
```

**Benefits:**
- Fast iteration during development
- Only changed classes are recompiled
- Scales to large codebases with many classes
- Hash database persists between builds

## Workflow Example

### Step 1: Define a C++ Class

```cpp
// vector3.hpp
struct Vector3 {
    double x, y, z;

    double length() const {
        return std::sqrt(x*x + y*y + z*z);
    }
};
```

### Step 2: Create Binding (Single Line!)

```cpp
// vector3_binding.cpp
#include "mirror_bridge.hpp"
#include "vector3.hpp"

MIRROR_BRIDGE_MODULE(vector3,
    mirror_bridge::bind_class<Vector3>(m, "Vector3");
)
```

### Step 3: Build

```bash
./build_bindings.sh
```

The script:
- Detects the Vector3 class via reflection
- Generates getters/setters for x, y, z automatically
- Compiles to `build/vector3.so`
- Stores class hash for future incremental builds

### Step 4: Use from Python

```python
import vector3

v = vector3.Vector3()
v.x = 3.0
v.y = 4.0
v.z = 0.0
print(f"Vector: ({v.x}, {v.y}, {v.z})")
```

## Technical Deep Dive

### Compile-Time Member Discovery

The most sophisticated part of mirror_bridge is the automatic generation of Python property descriptors:

```cpp
template<typename T, std::size_t... Indices>
consteval auto generate_getsetters(std::index_sequence<Indices...>) {
    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr std::size_t count = members.size();

    std::array<PyGetSetDef, count + 1> getsets{};

    // For each index, create a PyGetSetDef entry
    auto populate_entry = [&]<std::size_t I>() {
        constexpr auto member = members[I];
        constexpr auto name = std::meta::identifier_of(member);

        getsets[I] = PyGetSetDef{
            .name = name,
            .get = py_getter<T, I>,
            .set = py_setter<T, I>,
            // ...
        };
    };

    (populate_entry.template operator()<Indices>(), ...);
    return getsets;
}
```

**How It Works:**
1. `nonstatic_data_members_of(^^T)` returns a compile-time list of members
2. `std::index_sequence` generates indices [0, 1, 2, ..., N-1]
3. Fold expression `(expr, ...)` applies `populate_entry` for each index
4. Each entry gets a unique getter/setter templated on the index
5. The index is used to access the correct member via `[:member:]` reflection syntax

This is entirely resolved at compile-time - no runtime dispatch or lookup needed.

### Member Access via Reflection

```cpp
template<typename T, std::size_t Index>
PyObject* py_getter(PyObject* self, void* closure) {
    auto* wrapper = reinterpret_cast<PyWrapper<T>*>(self);

    constexpr auto members = std::meta::nonstatic_data_members_of(^^T);
    constexpr auto member = members[Index];

    // [:member:] is the reflection access syntax
    auto& value = wrapper->cpp_object->[:member:];
    return to_python(value);
}
```

The `[:member:]` syntax is P2996's way of "splicing" reflection metadata back into code. It's equivalent to writing `obj.x` but computed at compile-time from reflection metadata.

### Zero-Overhead Guarantee

**No runtime costs:**
- Member lookup: Direct offset access (same as native C++)
- Type conversion: Inline, branch-free for common types
- Object creation: Single allocation, no indirection
- Method dispatch: Not yet implemented, but will use function pointers (no vtables)

**Compare to alternatives:**
- pybind11: Requires manual specification of every member
- Boost.Python: Heavy template instantiation, slower compile times
- SWIG: External code generator, less type-safe
- ctypes: Manual ABI management, error-prone

## Limitations and Future Work

### Current Limitations

1. **Method Binding Not Yet Implemented**
   - Currently only data members are bound
   - Methods require additional reflection support for signatures
   - Planned for next version using `std::meta::member_functions_of`

2. **Basic Type Support**
   - Arithmetic types and strings are supported
   - Containers (vector, map, etc.) need concept-based handlers
   - Custom types require recursive binding

3. **Constructor Limitations**
   - Only default constructor is currently exposed
   - Need to reflect on constructors for parameterized creation

4. **No Inheritance Support**
   - Base classes are not automatically bound
   - Need to walk inheritance hierarchy via reflection

### Future Enhancements

**Method Binding:**
```cpp
constexpr auto methods = std::meta::member_functions_of(^^T);
// Generate PyMethodDef array similar to getsetters
```

**Container Support via Concepts:**
```cpp
template<typename T>
concept SequenceContainer = requires(T t) {
    { t.size() } -> std::convertible_to<std::size_t>;
    { t.begin() } -> std::forward_iterator;
    { t.end() } -> std::forward_iterator;
};

template<SequenceContainer T>
PyObject* to_python(const T& container) {
    PyObject* list = PyList_New(container.size());
    // Convert each element...
}
```

**Automatic Documentation:**
```cpp
// Extract comments via reflection metadata
constexpr auto doc = std::meta::documentation_of(^^T);
// Populate Python's __doc__ strings
```

## Docker Environment

The project includes a Docker setup for easy compilation with the experimental reflection compiler:

```dockerfile
# Builds clang-p2996 from source
FROM ubuntu:22.04
RUN git clone --branch p2996 https://github.com/bloomberg/clang-p2996.git
# ... build and install
```

**Why Docker?**
- Reflection support requires experimental compiler
- Building clang from source takes 30-60 minutes
- Docker provides reproducible environment
- No impact on host system

**Usage:**
```bash
./docker_build.sh                              # Build image (once)
docker run -v $(pwd):/workspace -it mirror_bridge:latest
# Inside container:
cd /workspace/tests
../build_bindings.sh                           # Compile bindings
python3 test_vector3.py                        # Run tests
```

## Comparison to RBB

Mirror Bridge is inspired by and shares goals with RBB (Reflection-Based Bindings), but with key differences:

**Similarities:**
- Both use C++26 reflection (P2996)
- Both target automatic binding generation
- Both emphasize zero-overhead abstractions

**Differences:**
1. **Single-header vs. Library Structure**
   - Mirror Bridge: One header, minimal dependencies
   - RBB: Full library with separate headers and implementation

2. **Incremental Compilation**
   - Mirror Bridge: Built-in hash-based selective recompilation
   - RBB: Standard build system (CMake)

3. **Registry System**
   - Mirror Bridge: Central registry with metadata tracking
   - RBB: Direct binding at module definition

4. **Target Scope**
   - Mirror Bridge: Starting with Python, expandable to Rust/Lua
   - RBB: Python-focused

## Conclusion

Mirror Bridge demonstrates that C++26 reflection enables truly automatic foreign function interfaces. By leveraging compile-time introspection and modern C++ concepts, we achieve:

- **Zero boilerplate**: One-line binding declarations
- **Zero overhead**: All work done at compile-time
- **Type safety**: Compiler-enforced correctness
- **Incremental builds**: Only changed classes recompile
- **Single-header simplicity**: Easy integration, no build complexity

The future of C++ interoperability is reflection-driven automation, and mirror_bridge provides a practical implementation of this vision.
