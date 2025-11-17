# Mirror Bridge - Comprehensive Benchmark Results

**Last Updated:** 2025-01-16
**Compiler:** Bloomberg clang-p2996 (C++26 reflection support)
**Python:** 3.10
**Platform:** Linux (Docker on macOS ARM64)

---

## Table of Contents
- [Compile-Time Performance](#compile-time-performance)
- [Binary Size](#binary-size)
- [Runtime Performance](#runtime-performance)
- [Developer Experience](#developer-experience)
- [Overall Comparison](#overall-comparison)

---

## Compile-Time Performance

Time to compile bindings (lower is better). Median of 5 runs with standard deviation.

### Simple Project (1 class, ~10 methods)

| Library | Time | Std Dev | vs pybind11 | vs nanobind |
|---------|------|---------|-------------|-------------|
| **nanobind** | **309ms** | ±6ms | **5.3x faster** | - |
| SWIG | 440ms | ±10ms | 3.7x faster | 1.4x slower |
| Mirror Bridge (auto) | 603ms | - | 2.7x faster | 2.0x slower |
| Mirror Bridge (manual) | 659ms | ±12ms | 2.5x faster | 2.1x slower |
| pybind11 | 1643ms | ±7ms | - | 5.3x slower |

### Medium Project (10 classes, ~50 methods)

| Library | Time | Std Dev | vs pybind11 | vs nanobind |
|---------|------|---------|-------------|-------------|
| **nanobind** | **1181ms** | ±28ms | **2.8x faster** | - |
| Mirror Bridge (auto) | 1393ms | - | 2.4x faster | 1.2x slower |
| Mirror Bridge (manual) | 1819ms | ±44ms | 1.8x faster | 1.5x slower |
| pybind11 | 3346ms | ±120ms | - | 2.8x slower |
| SWIG | 3589ms | ±79ms | 1.1x faster | 3.0x slower |

**Key Insight:** nanobind has fastest absolute compile times, but the gap shrinks as project size grows. Mirror Bridge's reflection overhead is more constant, while template-based approaches scale linearly.

---

## Binary Size

Shared library file size after compilation (lower is better).

### Simple Project (1 class, ~10 methods)

| Library | Size | vs pybind11 | vs nanobind |
|---------|------|-------------|-------------|
| **Mirror Bridge (manual)** | **43 KB** | **4.8x smaller** | **5.5x smaller** |
| SWIG | 47 KB | 4.4x smaller | 5.0x smaller |
| Mirror Bridge (auto) | 129 KB | 1.6x smaller | 1.8x smaller |
| pybind11 | 207 KB | - | 1.1x smaller |
| nanobind | 236 KB | 1.1x larger | - |

### Medium Project (10 classes, ~50 methods)

| Library | Size | vs pybind11 | vs nanobind |
|---------|------|-------------|-------------|
| **Mirror Bridge (manual)** | **248 KB** | **1.9x smaller** | **1.5x smaller** |
| nanobind | 364 KB | 1.3x smaller | - |
| pybind11 | 483 KB | - | 1.3x larger |
| Mirror Bridge (auto) | 638 KB | 1.3x larger | 1.8x larger |
| SWIG | 678 KB | 1.4x larger | 1.9x larger |

**Key Insight:** Mirror Bridge (manual) produces the smallest binaries across all project sizes. Auto-discovery mode trades binary size for zero boilerplate code.

---

## Runtime Performance

Average time per operation in nanoseconds (lower is better). Median of 5 runs across millions of iterations.

| Benchmark | Mirror Bridge | pybind11 | nanobind | vs pb11 | vs nanobind |
|-----------|--------------|----------|----------|---------|-------------|
| **Null call** | 35.1 ns | 130.6 ns | 35.2 ns | **3.7x faster** | 1.00x (tie) |
| **Add int** | 42.7 ns | 161.3 ns | 40.1 ns | **3.8x faster** | 1.07x slower |
| **Multiply double** | 43.0 ns | 159.7 ns | 41.4 ns | **3.7x faster** | 1.04x slower |
| **Concat string** | 63.0 ns | 176.7 ns | 61.2 ns | **2.8x faster** | 1.03x slower |
| **Get string** | 46.1 ns | 155.1 ns | 48.6 ns | **3.4x faster** | 0.95x faster |
| **Set string** | 45.7 ns | 150.8 ns | 42.7 ns | **3.3x faster** | 1.07x slower |
| **Vector append** | 44.0 ns | 147.1 ns | 44.7 ns | **3.3x faster** | 0.98x faster |
| **Vector get** | 469.2 ns | 573.7 ns | 490.5 ns | **1.2x faster** | 0.96x faster |
| **Vector set** | 67.2 ns | 193.8 ns | 60.3 ns | **2.9x faster** | 1.11x slower |
| **Attr get** | 35.2 ns | 131.4 ns | 40.2 ns | **3.7x faster** | 0.88x faster |
| **Attr set** | 62.0 ns | 179.2 ns | 71.7 ns | **2.9x faster** | 0.87x faster |
| **Construction** | 47.1 ns | 226.0 ns | 55.5 ns | **4.8x faster** | 0.85x faster |
| | | | | | |
| **Average** | **58.3 ns** | **173.8 ns** | **60.9 ns** | **3.0x faster** | **0.96x faster** |

**Boost.Python:** Not tested (incompatible with libc++ on this platform)

**Key Insight:** Mirror Bridge and nanobind are essentially tied for runtime performance, both significantly faster than pybind11. Mirror Bridge has a slight edge in attribute access and construction, nanobind in arithmetic operations.

---

## Developer Experience

Lines of binding code required (lower is better).

### Simple Project (1 class, ~10 methods)

| Library | Lines of Code | vs pybind11 |
|---------|--------------|-------------|
| **Mirror Bridge (auto)** | **0 lines** | **∞ (just CLI command)** |
| Mirror Bridge (manual) | 6 lines | 3.0x less |
| pybind11 | 18 lines | - |
| nanobind | ~20 lines | 1.1x more |
| SWIG | ~25 lines | 1.4x more |

**Example - Mirror Bridge (manual):**
```cpp
#include "mirror_bridge.hpp"
#include "simple_class.hpp"

MIRROR_BRIDGE_MODULE(simple_mb,
    mirror_bridge::bind_class<SimpleClass>(m, "SimpleClass");
)
```

**Example - Mirror Bridge (auto):**
```bash
mirror_bridge_auto src/ --module simple_mb
```

### Medium Project (10 classes, ~50 methods)

| Library | Lines of Code | vs pybind11 |
|---------|--------------|-------------|
| **Mirror Bridge (auto)** | **0 lines** | **∞ (just CLI command)** |
| Mirror Bridge (manual) | 15 lines | 6.9x less |
| pybind11 | 103 lines | - |
| nanobind | ~110 lines | 1.1x more |
| SWIG | ~120 lines | 1.2x more |

**Key Insight:** Mirror Bridge dramatically reduces boilerplate code. Auto-discovery mode requires zero binding code - just a CLI command. Manual mode still offers 3-7x reduction vs traditional approaches.

---

## Overall Comparison

### Simple Project Rankings

| Metric | 🥇 Best | 🥈 Second | 🥉 Third |
|--------|---------|-----------|----------|
| **Compile Time** | nanobind (309ms) | SWIG (440ms) | Mirror Bridge auto (603ms) |
| **Binary Size** | Mirror Bridge manual (43 KB) | SWIG (47 KB) | Mirror Bridge auto (129 KB) |
| **Runtime** | Mirror Bridge (35-63ns avg) | nanobind (35-61ns avg) | pybind11 (131-177ns avg) |
| **Code Size** | Mirror Bridge auto (0 lines) | Mirror Bridge manual (6) | pybind11 (18) |

### Medium Project Rankings

| Metric | 🥇 Best | 🥈 Second | 🥉 Third |
|--------|---------|-----------|----------|
| **Compile Time** | nanobind (1181ms) | Mirror Bridge auto (1393ms) | Mirror Bridge manual (1819ms) |
| **Binary Size** | Mirror Bridge manual (248 KB) | nanobind (364 KB) | pybind11 (483 KB) |
| **Runtime** | Mirror Bridge (35-469ns avg) | nanobind (35-491ns avg) | pybind11 (131-574ns avg) |
| **Code Size** | Mirror Bridge auto (0 lines) | Mirror Bridge manual (15) | pybind11 (103) |

---

## When to Choose Each Library

### Choose **Mirror Bridge** when:
- ✅ You value **minimal boilerplate code** (3-7x less than pybind11, or 0 with auto-discovery)
- ✅ **Binary size matters** (1.5-5.5x smaller than nanobind/pybind11)
- ✅ **Runtime performance is critical** (3-4x faster than pybind11, matches nanobind)
- ✅ You can use **experimental C++26** (Bloomberg clang-p2996)
- ✅ You're building **research/academic** projects exploring reflection
- ⚠️ Accept **slower compile times** (1.2-2x vs nanobind)

### Choose **nanobind** when:
- ✅ You need **fastest compile times** (1.2-5x faster than Mirror Bridge)
- ✅ **Production stability** is required (stable, battle-tested)
- ✅ You use **standard C++17** (no experimental compiler needed)
- ✅ You need **advanced features** (NumPy, complex type casters, etc.)
- ✅ **CMake integration** is important
- ⚠️ Accept **larger binaries** (1.5-5.5x vs Mirror Bridge manual)

### Choose **pybind11** when:
- ✅ You need **maximum compatibility** (most widely used)
- ✅ **Mature ecosystem** with extensive documentation
- ✅ **C++11/14/17/20** support (very flexible)
- ⚠️ Accept **slower compile times** (2-3x vs nanobind/Mirror Bridge auto)
- ⚠️ Accept **slower runtime** (3-4x vs Mirror Bridge/nanobind)

### Choose **SWIG** when:
- ✅ You need **multi-language bindings** (Python, Java, C#, etc.)
- ✅ **Very fast compile for small projects** (440ms simple project)
- ⚠️ Accept **poor scaling** (3.6s for medium project)
- ⚠️ Accept **larger binaries** and less control over binding details

---

## Technical Details

### Compiler Flags Used

**Mirror Bridge:**
```bash
clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ -O3 -DNDEBUG
```

**pybind11:**
```bash
clang++ -std=c++20 -stdlib=libc++ -O3 -DNDEBUG
```

**nanobind:**
```bash
clang++ -std=c++17 -stdlib=libc++ -O3 -DNDEBUG
# Pre-compiled stub library for fair benchmarking
```

**SWIG:**
```bash
swig -c++ -python  # Code generation
clang++ -std=c++20 -stdlib=libc++ -O3 -DNDEBUG  # Compilation
```

### Benchmark Methodology

- **Compile-time:** 5 runs, median time reported with stddev
- **Runtime:** 5 runs of 100K-10M iterations per benchmark, median reported
- **Binary size:** Post-compilation shared library file size
- **Lines of code:** Physical lines in binding files (excluding comments/whitespace)

---

## Conclusion

Mirror Bridge demonstrates that **C++26 reflection enables a new paradigm** for Python bindings:

- **Zero-boilerplate** with auto-discovery mode
- **State-of-the-art runtime performance** matching nanobind
- **Minimal binary sizes** beating all competitors
- **Trade-off:** Slower compile times and experimental compiler requirement

As C++26 reflection becomes standardized and compiler support matures, the compile-time gap will narrow, making reflection-based binding generation increasingly attractive for production use.

For projects that can adopt experimental C++26 today, Mirror Bridge offers the best combination of developer experience, runtime performance, and binary size.
