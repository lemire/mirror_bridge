# Mirror Bridge

**Modern C++ meets Python: Automatic bindings using C++26 reflection - zero boilerplate, pure compile-time magic.**

```cpp
// 1. Write modern C++ with reflection
struct Calculator {
    double value = 0.0;

    Calculator() = default;
    Calculator(double initial) : value(initial) {}

    double add(double x) { return value += x; }
    double subtract(double x) { return value -= x; }
};

// 2. One command to bind everything:
//    mirror_bridge_auto src/ --module math
```

```python
# 3. Use from Python - everything just works!
import math

calc = math.Calculator(100.0)  # Constructors with parameters ✓
calc.add(10)                   # Methods ✓
calc.subtract(3)               # Returns: 107.0
print(calc.value)              # Direct member access ✓
```

**No manual binding code. No wrapper macros. Just pure C++26 reflection.** 🎉

## What is Mirror Bridge?

Mirror Bridge is a **single-header library** that uses C++26 reflection (P2996) to automatically introspect your C++ classes at compile-time and generate Python bindings. It discovers:

- ✅ **Data members** - automatic getters/setters with type safety
- ✅ **Methods** (any number of parameters) - variadic parameter support
- ✅ **Constructors** - including parameterized constructors
- ✅ **Method overloading** - automatic name mangling for overloads
- ✅ **Smart pointers** - `std::unique_ptr`, `std::shared_ptr` with automatic conversion
- ✅ **Nested classes** - recursive handling, cross-file dependencies
- ✅ **Containers** - `std::vector`, `std::array` with bidirectional conversion
- ✅ **Exception handling** - C++ exceptions → Python exceptions

**Zero overhead:** All binding code is generated at compile-time through template metaprogramming and reflection - no runtime costs.

## Quick Start

### 5-Minute Tutorial

**1. Get the environment (one-time setup)**
```bash
./start_dev_container.sh
# First run: ~30-60 min to build clang-p2996
# Future runs: instant
```

**2. Inside container - verify it works**
```bash
cd /workspace
./tests/run_all_tests.sh
# Expected: ✓ ALL TESTS PASSED! (12/12)
```

**3. Try an example**
```bash
cd examples/option2
cat src/calculator.hpp  # Just a normal C++ class

# Generate bindings automatically
../../mirror_bridge_auto src/ --module math_module

# Use from Python
python3 test_option2.py
```

**4. Create your own**
```bash
cd /workspace

# Write a C++ header
cat > person.hpp << 'EOF'
struct Person {
    std::string name;
    int age;

    Person() = default;
    Person(std::string n, int a) : name(n), age(a) {}

    int birth_year(int current_year) { return current_year - age; }
};
EOF

# Create binding (one line!)
cat > person_binding.cpp << 'EOF'
#include "mirror_bridge.hpp"
#include "person.hpp"

MIRROR_BRIDGE_MODULE(people,
    mirror_bridge::bind_class<Person>(m, "Person");
)
EOF

# Compile
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -I. -fPIC -shared $(python3-config --includes --ldflags) \
    person_binding.cpp -o build/people.so

# Use from Python
python3 << 'EOF'
import sys; sys.path.insert(0, 'build')
import people

p = people.Person("Alice", 30)
print(f"{p.name} was born in {p.birth_year(2024)}")  # Alice was born in 1994
EOF
```

That's it! See [CONTRIBUTING.md](CONTRIBUTING.md) for detailed development guide.

## Two Approaches to Auto-Generation

Mirror Bridge offers two workflows optimized for different use cases:

### Option 1: Auto-Discovery (Minimal Friction)

**Just point at a directory - bindings are auto-generated for all classes.**

```bash
mirror_bridge_auto src/ --module my_module
```

- ✅ **Zero configuration** - discovers all classes automatically
- ✅ **Perfect for prototyping** and small projects
- ✅ **Opt-out via comments** - mark classes to skip

**Example:**
```cpp
// src/calculator.hpp
struct Calculator {
    double value;
    double add(double x);
};

// src/vector3.hpp
struct Vector3 {
    double x, y, z;
    double length();
};
```

```bash
# One command binds BOTH classes
mirror_bridge_auto src/ --module math
```

```python
import math
calc = math.Calculator()
vec = math.Vector3()
```

See [`examples/option2/`](examples/option2) for full example.

### Option 2: Config File (Production)

**Declarative config for explicit control over what gets bound.**

Create `my_module.mirror`:
```
module: my_module

include_dirs: src/, include/

Calculator: calculator.hpp
Vector3: vector3.hpp
```

```bash
mirror_bridge_generate my_module.mirror
```

- ✅ **Explicit control** - only bind what you specify
- ✅ **Version control friendly** - declarative config
- ✅ **Class renaming** - `Foo::Bar: foo.hpp as Bar`

See [`examples/option3/`](examples/option3) for full example.

**Full comparison:** [examples/README.md](examples/README.md)

## What Gets Auto-Generated?

### Data Members
```cpp
struct Point { double x, y; };
```
```python
p = my_module.Point()
p.x = 3.0  # Automatic getter/setter
p.y = 4.0
```

### Methods (Variadic Parameters)
```cpp
struct MathOps {
    double add3(double a, double b, double c) { return a + b + c; }
    double sum5(double a, double b, double c, double d, double e) {
        return a + b + c + d + e;
    }
    void reset() { value = 0; }  // Zero parameters work too
};
```
```python
ops = my_module.MathOps()
ops.add3(1.0, 2.0, 3.0)           # 3 parameters ✓
ops.sum5(1, 2, 3, 4, 5)           # 5 parameters ✓
ops.reset()                        # 0 parameters ✓
# ANY number of parameters supported through variadic templates
```

### Constructors with Parameters
```cpp
struct Rectangle {
    Rectangle() : width(0), height(0) {}
    Rectangle(double w, double h) : width(w), height(h) {}
    Rectangle(double w, double h, std::string name)
        : width(w), height(h), name(name) {}

    double width, height;
    std::string name;
};
```
```python
r1 = my_module.Rectangle()              # Default constructor
r2 = my_module.Rectangle(10.0, 5.0)    # 2-parameter constructor
r3 = my_module.Rectangle(10, 5, "box") # 3-parameter constructor
# Automatic constructor discovery and parameter matching
```

### Method Overloading
```cpp
struct Printer {
    void print(int value) { /* ... */ }
    void print(double value) { /* ... */ }
    void print(std::string value) { /* ... */ }
};
```
```python
p = my_module.Printer()
p.print_int(42)                    # int overload
p.print_double(3.14)               # double overload
p.print_std__string("hello")       # string overload
# Automatic name mangling distinguishes overloads
```

### Smart Pointers
```cpp
struct Data {
    std::string name;
    int value;
};

struct ResourceManager {
    std::unique_ptr<Data> unique_data;
    std::shared_ptr<Data> shared_data;

    std::unique_ptr<Data> create_unique(std::string n, int v);
};
```
```python
rm = my_module.ResourceManager()

# Smart pointers convert to/from Python dicts
result = rm.create_unique("test", 42)
print(result)  # {'name': 'test', 'value': 42}

# Set from dict - creates managed pointer automatically
rm.unique_data = {'name': 'data', 'value': 123}

# None handling for null pointers
rm.unique_data = None  # Sets to nullptr
```

### Nested Classes
```cpp
struct Address {
    std::string city;
};

struct Person {
    std::string name;
    Address addr;  // Nested!
};
```
```python
p = my_module.Person()
p.addr = {'city': 'Boston'}  # Dict conversion
```

### Containers
```cpp
struct Data {
    std::vector<double> values;
    std::array<int, 3> coords;
};
```
```python
d.values = [1.0, 2.0, 3.0]  # List → vector
d.coords = [1, 2, 3]         # List → array
```

### Exception Handling
```cpp
double divide(double x) {
    if (x == 0) throw std::runtime_error("Division by zero");
    return value / x;
}
```
```python
try:
    calc.divide(0)
except RuntimeError as e:
    print(e)  # "Division by zero"
```

## Project Structure

```
mirror_bridge/
├── mirror_bridge.hpp           # Single-header library (core reflection logic)
├── mirror_bridge_auto          # Auto-discovery script
├── mirror_bridge_generate      # Config file script
├── mirror_bridge_build         # Direct compilation script
├── start_dev_container.sh      # Docker setup (persistent container)
├── examples/
│   ├── README.md               # Detailed usage guide
│   ├── option2/                # Auto-discovery example
│   └── option3/                # Config file example
└── tests/
    ├── run_all_tests.sh        # Automated test suite
    └── e2e/                    # End-to-end tests
        ├── basic/              # Point2D, Vector3
        ├── containers/         # std::vector, std::array
        ├── nesting/            # Nested classes, cross-file
        └── methods/            # Method binding (Calculator)
```

## How It Works

Mirror Bridge leverages C++26 reflection at compile-time:

1. **Discovery**: Uses `std::meta::nonstatic_data_members_of(^^T)` to find all class members
2. **Method Introspection**: Uses `std::meta::members_of` + `std::meta::is_function` to find methods
3. **Type Extraction**: Uses `std::meta::type_of` and `std::meta::identifier_of` for names
4. **Code Generation**: Generates Python C API bindings via template metaprogramming
5. **Compilation**: Compiles to `.so` module with reflection-enabled clang

**All binding logic is resolved at compile-time - zero runtime overhead.**

See [CONTRIBUTING.md](CONTRIBUTING.md#architecture-overview) for technical details.

## Requirements

- **Compiler**: Bloomberg clang-p2996 (P2996 reflection support)
  - Provided via Docker: `./start_dev_container.sh`
  - Or build from: https://github.com/bloomberg/clang-p2996
- **Python**: 3.7+
- **Platform**: Linux (or macOS with Docker)

## Testing

```bash
# Inside Docker container:

# Run all automated tests
./tests/run_all_tests.sh

# Output:
# ✓ Built: 12 bindings
# ✓ Passed: 12 tests
# ✓ ALL TESTS PASSED!

# Test coverage:
# - Basic data members (Point2D, Vector3)
# - Containers (std::vector, std::array)
# - Nested classes (2-level, 3-level, cross-file)
# - Methods (Calculator with various signatures)
# - Variadic parameters (3, 4, 5, 6 parameter methods)
# - Constructors with parameters (0, 2, 3 parameters)
# - Method overloading (int/double/string overloads)
# - Smart pointers (unique_ptr, shared_ptr conversion)
```

**Advanced feature tests** ([`tests/e2e/advanced/`](tests/e2e/advanced/)):
- **Variadic**: Methods with 3-6 parameters, weighted sums, format functions
- **Constructors**: Default, 2-param, 3-param constructor matching
- **Overloading**: Type-based name mangling for overloaded methods
- **Smart Pointers**: Bidirectional dict conversion, nullptr handling, return values

## Documentation

- **[CONTRIBUTING.md](CONTRIBUTING.md)** - Development guide: setup, testing, CLI tools, architecture
- **[examples/README.md](examples/README.md)** - Usage examples and workflow comparisons
- **API Reference** - Inline documentation in [`mirror_bridge.hpp`](mirror_bridge.hpp)

## Current Limitations

- Passing bound class instances as parameters (requires reference/pointer handling)
- Template classes (must be explicitly instantiated before binding)
- Const method overloads (treated as same method currently)
- Advanced smart pointers (`weak_ptr`, custom deleters)

## Roadmap

**Recently Completed:**
- ✅ Variadic parameter support (any number of parameters)
- ✅ Constructor parameter binding
- ✅ Method overloading via name mangling
- ✅ Smart pointer support (`unique_ptr`, `shared_ptr`)

**Future:**
- [ ] Reference parameters and bound class passing
- [ ] Const method overload distinction
- [ ] Template class binding automation
- [ ] Performance benchmarks vs pybind11
- [ ] Additional backends (Rust, Lua)
- [ ] Python stub generation (.pyi files)

## Contributing

This is an experimental project exploring C++26 reflection. Contributions welcome!

**Areas needing work:**
- Extended parameter support for methods
- Template class handling
- Benchmark suite
- Additional backends (Rust, Lua)

## License

Apache License 2.0 - See [LICENSE](LICENSE) for details.

## Acknowledgments

- [Bloomberg's clang-p2996](https://github.com/bloomberg/clang-p2996) - P2996 reflection implementation
- [P2996 Reflection Proposal](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2996r7.html)
- simdjson - Concept-based design inspiration

---

**Status**: Experimental - C++26 reflection is not yet standardized. This project uses Bloomberg's clang-p2996 implementation.

**Yes, method binding works!** See calculator tests for full examples.
