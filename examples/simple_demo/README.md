# Simple Calculator Demo - Python to C++ with Zero Binding Code

This example shows how to speed up Python code with C++ using **Mirror Bridge** - without writing any binding code!

## The Idea

You have a Python calculator that works great, but it's slow for compute-intensive tasks. You want C++ speed without the hassle of writing bindings.

## What's Included

1. **calculator.hpp** - Simple C++ calculator with math operations
2. **calculator_python.py** - Equivalent Python implementation
3. **benchmark_calculator.py** - Performance comparison

## Quick Start

### Inside Docker (for auto-discovery)

```bash
# Start the Docker container
docker exec -it mirror_bridge_dev bash
cd /workspace/examples/simple_demo

# Generate C++ bindings automatically
../../mirror_bridge_auto . --module calculator

# Copy binding for easier import
cp build/calculator.so .

# Run the benchmark
python3 benchmark_calculator.py
```

## Results

**Real benchmark results:**

```
Operation                       Python          C++    Speedup
----------------------------------------------------------------------
prime_check                     0.014s       0.003s       5.3x
fibonacci                       0.497s       0.024s      20.5x
factorial                       0.008s       0.001s      11.1x
trig_functions                  0.021s       0.018s       1.2x
logarithms                      0.014s       0.012s       1.1x
gcd                             0.009s       0.004s       2.1x
----------------------------------------------------------------------
TOTAL                           0.618s       0.181s       3.4x
```

**🎉 C++ is 3.4x faster overall, with Fibonacci being 20.5x faster!**

## The Magic

### Before (Manual Bindings)

With traditional binding libraries, you'd write ~50-100 lines of binding code like this:

```cpp
// pybind11 example
PYBIND11_MODULE(calculator, m) {
    py::class_<Calculator>(m, "Calculator")
        .def(py::init<>())
        .def("add", &Calculator::add)
        .def("subtract", &Calculator::subtract)
        .def("multiply", &Calculator::multiply)
        .def("divide", &Calculator::divide)
        .def("power", &Calculator::power)
        .def("square_root", &Calculator::square_root)
        // ... 15+ more methods ...
        .def("is_prime", &Calculator::is_prime)
        .def("fibonacci", &Calculator::fibonacci)
        .def("factorial", &Calculator::factorial);
}
```

**Every time you add a method to your C++ class, you must update the bindings!**

### After (Mirror Bridge)

```bash
mirror_bridge_auto . --module calculator
```

**That's it. Zero lines of binding code.**

Mirror Bridge uses C++26 reflection to automatically discover:
- All classes in your headers
- All public methods
- All parameters and return types

Then generates optimal Python bindings at compile-time.

## Why This Works

The speedup comes from:

1. **Compiled Code**: C++ is compiled to native machine code
2. **No Interpreter Overhead**: Direct execution without Python's bytecode interpreter
3. **Better Optimizations**: Compiler can inline, vectorize, and optimize loops
4. **Type Specialization**: No runtime type checking

## Use Cases

This approach is perfect for:

- ✅ **Math-heavy computations** (like this calculator)
- ✅ **Data processing pipelines**
- ✅ **Algorithm implementations** (sorting, searching, graph algorithms)
- ✅ **Simulations and numerical methods**
- ✅ **Any CPU-bound Python code**

## What's Next?

Try modifying `calculator.hpp` to add new methods:

```cpp
double my_custom_function(double x) const {
    return x * x + 2 * x + 1;
}
```

Then regenerate bindings:

```bash
mirror_bridge_auto . --module calculator
```

Your new method is automatically available in Python - no binding code needed!

```python
import calculator
calc = calculator.Calculator()
result = calc.my_custom_function(5.0)  # Works instantly!
```

## Learn More

- [Mirror Bridge Documentation](../../README.md)
- [Advanced Blog Post Demo](../blog_post_demo/) - Image processing example with 29x speedup
- [GitHub Repository](https://github.com/FranciscoThiesen/mirror_bridge)
