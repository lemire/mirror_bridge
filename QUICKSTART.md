# Quick Start Guide

Get Mirror Bridge running in 5 minutes. This guide assumes you want to try it out first, understand later.

## Step 1: Get the Environment (One-Time Setup)

```bash
# Clone the repository (if you haven't)
git clone <repository-url>
cd mirror_bridge

# Build and start the Docker container with the reflection compiler
./start_dev_container.sh

# First run takes ~30-60 minutes (building clang-p2996)
# Future runs are instant (container persists)
```

You're now inside a container with everything you need. The `/workspace` directory is your project root.

## Step 2: Run the Tests

```bash
# Verify everything works
cd /workspace
./tests/run_all_tests.sh
```

You should see: **✓ ALL TESTS PASSED! (12/12)**

## Step 3: Try the Examples

### Example 1: Auto-Discovery (Easiest)

```bash
cd /workspace/examples/option2

# Look at the C++ code
cat src/calculator.hpp  # Just a normal C++ class

# Generate bindings automatically
../../mirror_bridge_auto src/ --module math_module

# Run it!
python3 test_option2.py
```

**What just happened?**
- The tool scanned `src/` for all C++ classes
- Generated Python bindings for everything it found
- Compiled to `build/math_module.so`
- Python can now import and use your C++ class

### Example 2: Config File (More Control)

```bash
cd /workspace/examples/option3

# Look at the config file
cat math.mirror  # Simple declarative config

# Generate bindings from config
../../mirror_bridge_generate math.mirror

# Run it!
python3 test_option3.py
```

## Step 4: Create Your Own

Create a simple example:

```bash
cd /workspace

# Create a C++ header
cat > my_class.hpp << 'EOF'
#include <string>

struct Person {
    std::string name;
    int age;

    Person() : name(""), age(0) {}
    Person(std::string n, int a) : name(n), age(a) {}

    void greet() {
        // This will work from Python!
    }

    int get_birth_year(int current_year) {
        return current_year - age;
    }
};
EOF

# Create binding file
cat > my_binding.cpp << 'EOF'
#include "mirror_bridge.hpp"
#include "my_class.hpp"

MIRROR_BRIDGE_MODULE(people,
    mirror_bridge::bind_class<Person>(m, "Person");
)
EOF

# Compile it
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -I. -fPIC -shared \
    $(python3-config --includes --ldflags) \
    my_binding.cpp -o build/people.so

# Use from Python
python3 << 'EOF'
import sys
sys.path.insert(0, 'build')
import people

# Create a person
p = people.Person("Alice", 30)

# Access members
print(f"Name: {p.name}, Age: {p.age}")  # Direct access!

# Call methods
p.greet()                               # Zero parameters
year = p.get_birth_year(2024)          # With parameters
print(f"Born in: {year}")

# Modify members
p.age = 31
print(f"Updated age: {p.age}")
EOF
```

**Congratulations!** You've created your first binding from scratch.

## What's Next?

### Learn More About Features

Mirror Bridge automatically handles:
- **Any number of parameters**: Methods can have 0, 1, 5, 10+ parameters
- **Constructors**: Including parameterized constructors
- **Overloading**: Multiple methods with the same name but different types
- **Smart pointers**: `unique_ptr` and `shared_ptr` convert to/from dicts
- **Containers**: `std::vector`, `std::array` convert to/from lists
- **Nested classes**: Classes containing other classes

See examples in `tests/e2e/advanced/` for each feature.

### Choose Your Workflow

Three ways to create bindings:

1. **Auto-discovery** (`mirror_bridge_auto`): Point at a directory, binds everything
   - Best for: Quick prototyping, binding entire libraries
   - Example: `mirror_bridge_auto src/ --module my_module`

2. **Config file** (`mirror_bridge_generate`): Declarative `.mirror` config
   - Best for: Production use, version control, selective binding
   - Example: `mirror_bridge_generate my_module.mirror`

3. **Manual binding** (`mirror_bridge_build`): Write binding `.cpp` yourself
   - Best for: Custom control, complex scenarios
   - Example: `mirror_bridge_build my_binding.cpp`

See [TOOLS.md](TOOLS.md) for complete reference.

### Dive Deeper

- **[README.md](README.md)** - Feature overview and examples
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - How it works internally
- **[TESTING.md](TESTING.md)** - Writing and running tests
- **[DEVELOPMENT.md](DEVELOPMENT.md)** - Development workflow

## Common Issues

### "fatal error: 'meta' file not found"
You need to use `clang++` with reflection support. This is provided in the Docker container.

### "undefined symbol" linker errors
Make sure you're using the reflection compiler flags:
```bash
-std=c++2c -freflection -freflection-latest -stdlib=libc++
```

### Smart pointer compilation issues
Just bind both the container class and the element type:
```cpp
mirror_bridge::bind_class<Data>(m, "Data");
mirror_bridge::bind_class<Container>(m, "Container");  // Has unique_ptr<Data>
```

### Need help?
Check [DEVELOPMENT.md](DEVELOPMENT.md) for troubleshooting and advanced topics.

---

**You're ready to go!** Start binding your C++ code to Python with zero boilerplate.
