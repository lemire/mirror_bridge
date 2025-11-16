# Mirror Bridge - User Guide

Two ways to generate Python bindings from C++ using **C++26 reflection** - zero manual wrapping code!

## Quick Comparison

| Feature | Option 2: Auto-Discovery | Option 3: Config File |
|---------|-------------------------|----------------------|
| Setup | Just point to directory | Create .mirror file |
| Control | Automatic (all classes) | Explicit class list |
| Best for | Rapid prototyping | Production projects |
| Module organization | Single module | Can specify modules |
| Command | `mirror_bridge_auto` | `mirror_bridge_generate` |

---

## Option 2: Auto-Discovery (Minimal Friction)

**Best for:** Quick starts, demos, binding everything in a directory

### Usage

1. Organize your headers in a directory:
```
src/
  calculator.hpp
  vector3.hpp
  my_class.hpp
```

2. Run **one command**:
```bash
mirror_bridge_auto src/ --module my_module
```

3. Import everything:
```python
import sys
sys.path.insert(0, 'build')
import my_module

# All classes in src/ are automatically available!
calc = my_module.Calculator()
vec = my_module.Vector3()
obj = my_module.MyClass()
```

### Opt-Out (Skip Specific Classes)

Add a comment before classes you don't want to bind:

```cpp
// Regular class - will be auto-discovered
struct PublicAPI {
    double value;
};

// MIRROR_BRIDGE_SKIP
struct InternalHelper {
    // This won't be bound
};
```

Or skip entire files:

```cpp
// MIRROR_BRIDGE_SKIP_FILE
// Everything in this file will be ignored
```

### Pros & Cons
✅ **Zero configuration** - just point at directory
✅ **Fastest setup** - one command
✅ **Perfect for prototyping**
✅ **Automatically includes new classes**
❌ Less control over what gets bound
❌ Everything in one module

---

## Option 3: Config File (Production)

**Best for:** Larger projects, multiple modules, explicit control

### Usage

1. Create a config file (`bindings.mirror`):
```
# Module name
module: math_module

# Include directories
include_dirs: src/, include/

# Explicit class list
Calculator: calculator.hpp
Vector3: vector3.hpp
MyNamespace::Complex: complex.hpp as Complex
```

2. Generate:
```bash
mirror_bridge_generate bindings.mirror
```

3. Import:
```python
import sys
sys.path.insert(0, 'build')
import math_module

calc = math_module.Calculator()
vec = math_module.Vector3()
```

### Config File Syntax

```
# Comments start with #

# Required: module name
module: my_module_name

# Optional: include directories (comma-separated, relative to config file)
include_dirs: src/, include/, ../common/

# Class bindings
# Format: ClassName: header_file [as PythonName]
Calculator: calculator.hpp
Vector3: vector3.hpp
namespace::MyClass: my_class.hpp as MyClass
```

### Pros & Cons
✅ **Explicit control** over what gets bound
✅ **Declarative** - easy to review
✅ **Version control friendly**
✅ **Can rename** classes for Python
❌ Requires maintaining config file
❌ Must manually add new classes

---

## What Gets Auto-Generated?

Mirror Bridge uses **C++26 reflection** to automatically create bindings for:

### ✅ Data Members
```cpp
struct Calculator {
    double value;
};
```
```python
calc.value = 42.0  # Automatic getter/setter!
```

### ✅ Methods (0-2 parameters)
```cpp
struct Calculator {
    double add(double x) { return value += x; }
    void reset() { value = 0; }
    double compute(double x, double y) { return (x + y) * 2; }
};
```
```python
calc.add(5)          # Return values
calc.reset()         # Void methods
calc.compute(10, 5)  # Multiple parameters
```

### ✅ Const Methods
```cpp
double get_value() const { return value; }
```
```python
val = calc.get_value()  # Works!
```

### ✅ Exception Handling
```cpp
if (x == 0) throw std::runtime_error("Error");
```
```python
try:
    calc.divide(0)
except RuntimeError as e:
    print(e)  # "Error"
```

### ✅ Nested Classes & Cross-File Dependencies
```cpp
// address.hpp
struct Address {
    std::string city;
};

// university.hpp
#include "address.hpp"
struct University {
    Address location;  // Cross-file nesting!
};
```
```python
uni.location = {'city': 'Boston'}  # Converted to dict
```

### ✅ Containers
```cpp
std::vector<double> values;
std::array<int, 3> coords;
```
```python
obj.values = [1.0, 2.0, 3.0]  # Automatic conversion!
```

---

## Examples

### Option 2: Auto-Discovery
```bash
cd examples/option2

# One command to bind everything in src/
mirror_bridge_auto src/ --module math_module

# Test it
python3 test_option2.py
```

### Option 3: Config File
```bash
cd examples/option3

# Generate from config
mirror_bridge_generate math.mirror

# Test it
python3 test_option3.py
```

---

## When to Use Which?

### Use Option 2 (Auto-Discovery) when:
- 🚀 Starting a new project
- 🔬 Prototyping/experimenting
- 📦 Binding a library with <20 classes
- ⚡ You want the fastest setup
- 🎯 All classes in a directory should be bound

### Use Option 3 (Config File) when:
- 🏢 Production code
- 📚 Large codebase (20+ classes)
- 🎛️ Need fine-grained control
- 🔀 Want multiple modules
- 📝 Prefer declarative configuration
- 🔒 Need to review exactly what's exposed

---

## Current Limitations

- Methods with >2 parameters (not yet implemented)
- Passing bound class instances as parameters
- Method overloading (C++ reflection limitation)
- Template classes (must be explicitly instantiated)

---

## Requirements

- Bloomberg clang-p2996 (C++26 reflection compiler)
- Python 3.7+
- Linux/Docker environment

See main README for setup.

---

## Pro Tips

### Combining Both Approaches

Use auto-discovery during development, switch to config for production:

```bash
# Development: quick iteration
mirror_bridge_auto src/ --module my_module

# Production: explicit control
# Create my_module.mirror with discovered classes
mirror_bridge_generate my_module.mirror
```

### Keep Generated Files

Debug compilation issues by keeping the generated binding file:

```bash
mirror_bridge_auto src/ --module my_module --keep-generated
# Inspect build/my_module_binding.cpp
```

### Verbose Mode

See full compilation output:

```bash
mirror_bridge_auto src/ --module my_module --verbose
mirror_bridge_generate config.mirror --verbose
```

---

**No manual wrapping code. Ever.** 🎉

That's the power of C++26 reflection!
