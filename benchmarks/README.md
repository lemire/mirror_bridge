# Mirror Bridge Benchmarks

Comprehensive performance benchmarks comparing Mirror Bridge against pybind11 and Boost.Python.

## Benchmark Categories

### 1. Compile-Time Performance

Measures compilation speed and binary size for different project scales.

**Test Cases:**
- **Simple** (1 class, ~10 methods): Basic class with getters, setters, and methods
- **Medium** (10 classes, ~50 methods): Realistic 3D scene graph with interconnected classes

**Metrics:**
- Compilation time (milliseconds)
- Binary size (KB)
- Lines of binding code

### 2. Runtime Performance

Measures overhead of different operations at runtime.

**Test Cases:**
- **Null call**: Empty method call (pure overhead)
- **Arithmetic**: int and double operations
- **String operations**: Concatenation, get/set
- **Vector operations**: Append, get/set
- **Attribute access**: Get/set member variables
- **Object construction**: Creating new instances

**Iterations:**
- 10M iterations for simple ops (null call, arithmetic, attributes)
- 1M iterations for string ops
- 100K iterations for vector ops
- 1M iterations for object construction

### 3. Developer Experience

Measures how much code developers need to write.

**Metrics:**
- Lines of binding code
- Binding code complexity
- Maintainability (manual vs auto-generated)

## Running Benchmarks

### Quick Start (Recommended)

```bash
# From project root
./run_benchmarks.sh
```

This script will:
1. Build the benchmark Docker image (if not already built)
2. Run all benchmarks inside the container
3. Display results and save to JSON

**First run**: ~30-60 minutes (builds Docker image with clang + pybind11 + Boost.Python)
**Subsequent runs**: ~5-6 minutes (just runs benchmarks)

### Manual Run (Advanced)

If you want more control:

```bash
# Build benchmark image
docker build -f Dockerfile.benchmarks -t mirror_bridge:benchmarks .

# Run benchmarks
docker run --rm -v $(pwd):/workspace mirror_bridge:benchmarks bash -c \
  "cd /workspace/benchmarks && ./run_all_benchmarks.sh"
```

### What Gets Run

The benchmark suite will:
1. Build all binding modules (Mirror Bridge, pybind11, Boost.Python)
2. Measure compile times for simple and medium projects
3. Run runtime performance tests
4. Generate comparison report

### Expected Runtime

- Compile-time benchmarks: ~30 seconds
- Runtime benchmarks: ~3-5 minutes
- **Total: ~5-6 minutes**

## Understanding Results

### Compile-Time Results

Example output:
```
Simple Project (1 class, ~10 methods)
  Mirror Bridge...  850ms  (245 KB)
  pybind11...       1200ms (312 KB)
  Boost.Python...   3500ms (1200 KB)

  Speedup: Mirror Bridge vs pybind11:     1.41x
  Speedup: Mirror Bridge vs Boost.Python: 4.12x
```

**What to look for:**
- **Compilation time**: Lower is better (faster developer iteration)
- **Binary size**: Lower is better (smaller deployment footprint)
- **Speedup ratio**: Higher is better (Mirror Bridge should be faster)

### Runtime Results

Example output:
```
Benchmark            Mirror Bridge   pybind11        Boost.Python    vs pb11
--------------------------------------------------------------------------------
Null call            45 ns           42 ns           78 ns           1.07x ✓
Add int              48 ns           45 ns           82 ns           1.07x ✓
String concat        125 ns          118 ns          205 ns          1.06x ✓
```

**Legend:**
- ✓ within 10% of pybind11 (acceptable overhead)
- ⚠ within 50% of pybind11 (noticeable overhead)
- ✗ slower than 50% of pybind11 (significant overhead)

**What to look for:**
- Mirror Bridge should be **within 10%** of pybind11 for most operations
- Both should significantly outperform Boost.Python
- The "zero overhead" claim is validated if ratios are near 1.0x

### Developer Experience Results

Example output:
```
Lines of Binding Code

Simple project:
  Mirror Bridge:  6 lines
  pybind11:       18 lines
  Boost.Python:   21 lines

Medium project:
  Mirror Bridge:  16 lines
  pybind11:       120 lines
  Boost.Python:   135 lines
```

**What to look for:**
- Mirror Bridge should require **significantly less code** (5-10x reduction for medium projects)
- This demonstrates the value of automatic reflection-based binding

## Benchmark Implementation Details

### Compile-Time Benchmarks

Each binding is compiled 3 times and the median is reported to reduce variance from:
- Filesystem caching
- CPU frequency scaling
- Background processes

Caches are cleared between runs using `sync` and `drop_caches`.

### Runtime Benchmarks

Each operation is:
1. **Warmed up**: 1000 iterations (or 10% of total) to ensure JIT compilation
2. **Measured**: 5 runs of full iteration count
3. **Aggregated**: Median of 5 runs reported

This reduces variance from:
- Python JIT warmup
- CPU frequency scaling
- Garbage collection

### Statistical Rigor

- **Median** is used instead of mean to reduce impact of outliers
- **Multiple runs** ensure reproducibility
- **High iteration counts** reduce per-call measurement overhead
- **Warmup phase** ensures steady-state performance

## Interpreting Success

Mirror Bridge is considered successful if:

1. **Compile-time**: 1.2x-2x faster than pybind11 for medium projects
2. **Runtime**: Within 10% of pybind11 for most operations
3. **Developer Experience**: 5-10x less binding code for medium projects

The key value proposition is:
> **"Compile faster, write less code, run at the same speed"**

## Output Files

- `runtime_results.json`: Detailed runtime benchmark data (nanoseconds per operation)
- Console output: Human-readable summary with speedup ratios

## Customizing Benchmarks

To add new test cases:

1. **Compile-time**: Add new directory under `compile_time/` with C++ header + 3 binding files
2. **Runtime**: Add new method to `benchmark_class.hpp` and update `run_runtime_benchmarks.py`

To change iteration counts, edit `ITERATIONS` dictionary in `run_runtime_benchmarks.py`.

## Known Limitations

- Benchmarks assume Docker environment with reflection-enabled clang
- pybind11 and Boost.Python must be installed in container
- Some variance expected due to system load, run multiple times for confidence
- Binary size depends on optimization level and linking mode

## Contributing

When adding benchmarks:
1. Ensure all three implementations (MB, pb11, BP) are equivalent
2. Document what the benchmark measures
3. Choose appropriate iteration counts (balance accuracy vs runtime)
4. Include results in PR description
