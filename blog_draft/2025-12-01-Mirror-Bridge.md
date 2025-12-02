---
layout: post
title: mirror-bridge - making Python bindings for C++ frictionless
tags: Modern C++, Python, Bindings, Reflection, C++26, Performance
---

You have a Python codebase. Thousands of lines. It works. Your team knows it. Your tests cover it. Your CI/CD deploys it.

But there's this one function. It shows up in every profile. It's eating 80% of your runtime. You *know* it would be faster in C++.

The traditional answer? Rewrite it in C++, then spend sometime writing pybind11 boilerplate to call it from Python. Or just... don't bother.

**Mirror Bridge** is a third option: write C++, run one command, done.

## The Setup

Let's say you have a simple operation - a dot product:

```cpp
// vec3.hpp
struct Vec3 {
    double x, y, z;

    Vec3(double x, double y, double z) : x(x), y(y), z(z) {}

    double dot(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }
};
```

To use this from Python:

```bash
./mirror_bridge_auto src/ --module vec3 -o .
```

That's it. No binding code. Mirror Bridge uses C++26 reflection to discover your classes, methods, and fields automatically.

```python
import vec3

a = vec3.Vec3(1, 2, 3)
b = vec3.Vec3(4, 5, 6)
print(a.dot(b))  # 32.0
```

## The Naive Benchmark (And Why It's Misleading)

Let's call `dot()` a million times:

```python
for _ in range(1_000_000):
    a.dot(b)
```

**Results (M3 Max MacBook Pro):**
```
Python class: 0.11s
C++ via Mirror Bridge: 0.04s
Speedup: 2.9x
```

A ~3x speedup. Not bad, but not eye-popping. What's going on?

**This benchmark can be somewhat misleading**

Each call from Python to C++ pays a toll - argument conversion, language boundary crossing, result conversion. For a trivial operation like `dot()` (3 multiplies, 2 adds), that toll dominates the actual work.

This is like benchmarking a Ferrari by measuring how fast it can start and stop at every intersection. You're measuring overhead, not speed.

### Dissecting the Cross-Language Barrier

What actually happens when Python calls a C++ function? Let's trace through a single `a.dot(b)` call:

1. **Method lookup.** Python sees `a.dot` and searches for the `dot` attribute. It checks `a.__dict__`, then `type(a).__dict__`, walking up the MRO (Method Resolution Order). For our bound C++ class, this eventually finds a descriptor that wraps the C++ method.

2. **Argument boxing.** Python passes `b` as a `PyObject*` - a pointer to a heap-allocated structure containing a reference count, type pointer, and the actual data. The binding layer (nanobind, in Mirror Bridge's case) must extract the underlying C++ `Vec3` from this Python wrapper.

3. **Type checking.** Is `b` actually a `Vec3`? The binding layer verifies this at runtime. In pure C++, the compiler guarantees types at compile time - no runtime check needed.

4. **GIL management.** The Global Interpreter Lock might need to be considered. For simple numeric operations we keep it held, but for longer operations you'd release it to allow other Python threads to run.

5. **The actual call.** Finally, we call the C++ `dot()` method. This is the fast part - a few nanoseconds for 3 multiplies and 2 adds.

6. **Result conversion.** The C++ `double` result must be wrapped in a Python `float` object - another heap allocation, reference count initialization, and type pointer setup.

7. **Return.** Python receives a `PyObject*` pointing to the new float.

For a function that does microseconds of work, this overhead is negligible. For a function that does *nanoseconds* of work - like our `dot()` - the overhead dominates. You're spending more time crossing the border than doing actual computation.

### Why Is C++ Faster in the First Place?

Even setting aside the cross-language overhead, why is C++ inherently faster for the same operation? Let's look at what each language actually does for `dot()`:

**Python's journey:**

```python
def dot(self, other):
    return self.x*other.x + self.y*other.y + self.z*other.z
```

Each attribute access (`self.x`) involves:
- Dictionary lookup in `self.__dict__`
- If not found, search the class hierarchy
- Return a Python `float` object (heap-allocated, reference-counted)

Each multiplication:
- Check types of both operands at runtime
- Dispatch to the appropriate `__mul__` implementation
- Allocate a new Python `float` for the result

Each addition: same story. We can see this by looking at the Python bytecode:

```python
# Python bytecode for Vec3.dot() - run: python3 -c "import dis; dis.dis(Vec3.dot)"
  0 LOAD_FAST      0 (self)
  2 LOAD_ATTR      0 (x)      # dict lookup for self.x
  4 LOAD_FAST      1 (other)
  6 LOAD_ATTR      0 (x)      # dict lookup for other.x
  8 BINARY_MULTIPLY           # type check, dispatch, allocate result
 10 LOAD_FAST      0 (self)
 12 LOAD_ATTR      1 (y)      # dict lookup for self.y
 14 LOAD_FAST      1 (other)
 16 LOAD_ATTR      1 (y)      # dict lookup for other.y
 18 BINARY_MULTIPLY           # type check, dispatch, allocate result
 20 BINARY_ADD                # type check, dispatch, allocate result
 22 LOAD_FAST      0 (self)
 24 LOAD_ATTR      2 (z)      # dict lookup for self.z
 26 LOAD_FAST      1 (other)
 28 LOAD_ATTR      2 (z)      # dict lookup for other.z
 30 BINARY_MULTIPLY           # type check, dispatch, allocate result
 32 BINARY_ADD                # type check, dispatch, allocate result
 34 RETURN_VALUE
```

That's 18 bytecode instructions, each of which expands to many machine instructions. The `LOAD_ATTR` operations involve hash table lookups. The `BINARY_*` operations involve type checking and dynamic dispatch. By the end, we've done 6 attribute lookups, 5 arithmetic operations (each with type checks), and allocated several intermediate `float` objects.

**C++'s journey:**

```cpp
double dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}
```

The compiler knows at compile time:
- `x`, `y`, `z` are `double` values at fixed offsets from `this`
- `other.x`, `other.y`, `other.z` are at fixed offsets from `&other`
- All operations are `double * double` and `double + double`

Here's the [actual generated assembly](https://godbolt.org/z/Txhs9qd5G) (clang 18, -O3):

```asm
call_dot(Vec3 const&, Vec3 const&):
  movsd xmm1, qword ptr [rdi]       ; load a.x
  movsd xmm0, qword ptr [rdi + 8]   ; load a.y
  mulsd xmm0, qword ptr [rsi + 8]   ; a.y * b.y
  mulsd xmm1, qword ptr [rsi]       ; a.x * b.x
  addsd xmm1, xmm0                  ; (a.x*b.x) + (a.y*b.y)
  movsd xmm0, qword ptr [rdi + 16]  ; load a.z
  mulsd xmm0, qword ptr [rsi + 16]  ; a.z * b.z
  addsd xmm0, xmm1                  ; add all three
  ret                                ; result in xmm0
```

That's it. Four memory loads (some folded into `mulsd`), three multiplies, two adds, one return. No allocations. No type checks. No dictionary lookups. The entire function is 9 instructions.

This is why the "naive" benchmark shows only 3x speedup instead of 100x: Python's overhead for calling a function (even a Python function) is substantial, so replacing the *implementation* with C++ helps less than you'd expect. The win comes when you stop crossing the boundary repeatedly.

## The Real Benchmark: Surgical Optimization

In practice, you don't call C++ a million times from Python. You identify a hot loop and move *the entire loop* to C++.

Imagine you have this in your Python codebase:

```python
def hot_loop(n):
    """This function showed up in your profiler."""
    total = 0.0
    for i in range(n):
        v = Vec3(i * 0.1, i * 0.2, i * 0.3)
        total += v.dot(direction) / dir_len
    return total
```

It's called from dozens of places. The `Vec3` class is used throughout your codebase. You don't want to rewrite everything - you just want *this loop* to go fast.

So you write the C++ version:

```cpp
// Add to vec3.hpp
struct Vec3 {
    // ... existing code ...

    static double hot_loop(int n) {
        Vec3 direction(1, 1, 1);
        double dir_len = direction.length();
        double total = 0.0;
        for (int i = 0; i < n; ++i) {
            Vec3 v(i * 0.1, i * 0.2, i * 0.3);
            total += v.dot(direction) / dir_len;
        }
        return total;
    }
};
```

Rebuild: `./mirror_bridge_auto src/ --module vec3 -o . --force`

Now benchmark:

```python
# Python version
result = hot_loop(1_000_000)

# C++ version - ONE call, all work happens in C++
result = vec3.Vec3.hot_loop(1_000_000)
```

**Results (M3 Max MacBook Pro):**
```
Python: 0.26s
C++:    0.004s
Speedup: 67x
```

*That's* the real number. You pay the Python→C++ toll once. The loop runs at full native speed.

## Why Not Just Rewrite Everything in C++?

Because you probably shouldn't.

Python gives you:
- Rapid iteration (no compile step during development)
- A massive ecosystem (PyTorch, pandas, requests, FastAPI...)
- Readable glue code that connects everything
- Easy onboarding for new team members

The Pareto principle applies aggressively here. 80% of your runtime might come from 20% of your code. Often it's even more skewed - a single hot loop can dominate everything.

Mirror Bridge lets you surgically replace *just the hot 20%* with C++, keeping the other 80% in Python. You get:
- Python's ergonomics for the code that doesn't need to be fast
- C++ performance for the code that does
- Zero binding boilerplate connecting them

## The Magic: C++26 Reflection

Here's where it gets interesting. How does Mirror Bridge discover your classes automatically?

The answer is **C++26 static reflection** ([P2996](https://wg21.link/p2996)) - a new language feature that lets code inspect itself at compile time.

When you run `mirror_bridge_auto`, it generates code that looks something like this:

```cpp
// Auto-generated binding code (simplified)
#include "vec3.hpp"
#include <mirror_bridge.hpp>

MIRROR_BRIDGE_MODULE(vec3,
    mirror_bridge::bind_class<Vec3>(m, "Vec3");
)
```

The magic is in `bind_class<Vec3>`. Using reflection, Mirror Bridge can ask the compiler:

```cpp
// What Mirror Bridge does internally
constexpr auto type_info = ^Vec3;  // "reflection operator" - get metadata about Vec3

// Get all members of Vec3
constexpr auto members = std::meta::members_of(type_info);

// Iterate over them AT COMPILE TIME
template for (constexpr auto member : members) {
    // What's the name of this member?
    constexpr auto name = std::meta::identifier_of(member);

    // Is it public?
    if constexpr (std::meta::is_public(member)) {

        // Is it a function?
        if constexpr (std::meta::is_function(member)) {
            // Bind it as a Python method
            cls.def(name, /* pointer to member function */);
        }

        // Is it a data member?
        else if constexpr (std::meta::is_nonstatic_data_member(member)) {
            // Bind it as a Python attribute
            cls.def_readwrite(name, /* pointer to member */);
        }
    }
}
```

The `^` operator (called the "reflection operator") takes a type and returns a *compile-time value* representing that type's metadata. From there, `std::meta` functions let you query everything about it: members, their types, their names, whether they're public, static, const, etc.

This is fundamentally different from runtime reflection (like Java or C#). Everything happens at compile time:
- **Zero runtime overhead** - the reflection queries are resolved during compilation
- **Full type safety** - the compiler knows exact types, no casting or dynamic lookups
- **Works with templates** - you can reflect on template instantiations

Before P2996, generating bindings required either:
1. Manual listing (pybind11) - tedious and error-prone
2. Code parsing (SWIG) - fragile and limited
3. Macros (Boost.Python) - ugly and inflexible

Reflection gives us a fourth option: **ask the compiler directly**. It already knows everything about your types. Now we can access that knowledge programmatically.

## The Comparison

For context, here's what the traditional pybind11 approach requires:

```cpp
#include <pybind11/pybind11.h>
#include "vec3.hpp"

PYBIND11_MODULE(vec3, m) {
    py::class_<Vec3>(m, "Vec3")
        .def(py::init<double, double, double>())
        .def_readwrite("x", &Vec3::x)
        .def_readwrite("y", &Vec3::y)
        .def_readwrite("z", &Vec3::z)
        .def("dot", &Vec3::dot)
        .def("length", &Vec3::length)
        .def_static("hot_loop", &Vec3::hot_loop);
}
```

Every single method needs to be manually listed. And when you add a new one? Update the bindings. Forgot one? Silent failure at runtime.

With Mirror Bridge: write C++, run command, done.

## Getting Started

```bash
git clone https://github.com/FranciscoThiesen/mirror_bridge
cd mirror_bridge
./start_dev_container.sh  # Pre-built Docker image with clang-p2996

# Inside container - try the example from this post
cd /workspace/examples/blog_vec3
../../mirror_bridge_auto . --module vec3 -o .
python3 benchmark.py
```

The [`examples/blog_vec3`](https://github.com/FranciscoThiesen/mirror_bridge/tree/main/examples/blog_vec3) directory contains all the code from this post, ready to run.

The Docker image includes [clang-p2996](https://github.com/bloomberg/clang-p2996), Bloomberg's experimental Clang fork that implements the reflection proposal. As P2996 moves toward standardization, expect this to land in mainline compilers.

## Troubleshooting

**"fatal error: 'meta' file not found"**

The `<meta>` header is part of libc++ built with reflection support. If you see this error:

1. Make sure you're using the Docker container (`./start_dev_container.sh`)
2. If the pre-built image doesn't work for your architecture, choose option 2 to build from source
3. The build takes 45-90 minutes but only happens once

**Architecture mismatch**

If the pre-built image was built on a different architecture than yours (e.g., pulling an x86_64 image on ARM64), the `start_dev_container.sh` script will detect this and offer to rebuild from source.

**Docker build fails**

Common issues:
- **Out of disk space**: The build needs ~15GB. Run `docker system prune` to free space.
- **Out of memory**: Docker Desktop needs at least 8GB RAM allocated (Settings > Resources > Memory).
- **Network timeout**: Just retry - Docker caches progress.

## The Bottom Line

The next time you're staring at a Python profiler, wondering if it's worth the hassle to optimize that hot loop in C++, the answer is: **yes, and it should take less than five minutes**.

Write the C++ version. Run `mirror_bridge_auto`. Replace the call. Ship it.

---

- [Mirror Bridge on GitHub](https://github.com/FranciscoThiesen/mirror_bridge)
- [C++26 Reflection Proposal (P2996)](https://wg21.link/p2996)
- [clang-p2996](https://github.com/bloomberg/clang-p2996)

## Acknowledgments

Mirror Bridge wouldn't exist without the work of many others. Thanks to Wyatt Childers, Peter Dimov, Dan Katz, Barry Revzin, Andrew Sutton, Faisal Vali, and Daveed Vandevoorde for authoring and championing P2996. Special thanks to Dan Katz for maintaining the [clang-p2996](https://github.com/bloomberg/clang-p2996) branch that makes this all possible today.

The idea of using reflection to generate Python bindings isn't new - it was explored by others before, including in [this CppCon talk](https://www.youtube.com/watch?v=SJ0NFLpR9vE). Mirror Bridge builds on these ideas and packages them into a tool you can use today.


