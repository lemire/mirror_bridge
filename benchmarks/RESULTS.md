# Mirror Bridge Benchmark Results

## Status

**Benchmark Framework**: ✅ Complete
**Actual Benchmark Run**: ⏳ Pending (requires `mirror_bridge:benchmarks` Docker image)

The comprehensive benchmark suite is fully implemented and ready to run. This document describes the benchmark methodology and framework.

## Benchmark Coverage

### 1. Compile-Time Performance

Tests all three binding approaches across two project sizes:

| Project | Classes | Methods | Binding Approaches |
|---------|---------|---------|-------------------|
| **Simple** | 1 | ~10 | Manual, Auto-discovery, pybind11 |
| **Medium** | 10 | ~50 | Manual, Auto-discovery, pybind11 |

**Metrics Measured:**
- Compilation time (median of 3 runs)
- Binary size (.so file size in KB)
- Speedup ratio vs pybind11

**Optimization Flags**: Both Mirror Bridge and pybind11 compiled with `-O3 -DNDEBUG` for fair comparison.

### 2. Runtime Performance

12 micro-benchmarks measuring call overhead:

| Benchmark | Iterations | Purpose |
|-----------|-----------|---------|
| Null call | 10M | Pure binding overhead |
| Add int | 10M | Integer parameter passing |
| Multiply double | 10M | Float parameter passing |
| Concat string | 1M | String operations |
| Get string | 1M | String return values |
| Set string | 1M | String parameter passing |
| Vector append | 100K | Container modifications |
| Vector get | 100K | Container return values |
| Vector set | 100K | Container parameter passing |
| Attr get | 10M | Attribute access overhead |
| Attr set | 10M | Attribute modification overhead |
| Construction | 1M | Object creation overhead |

**Methodology**: Each benchmark runs 5 times with warmup phase, median time reported in nanoseconds per operation.

### 3. Developer Experience

Lines of code comparison:

| Project | Mirror Bridge (Manual) | Mirror Bridge (Auto) | pybind11 |
|---------|----------------------|---------------------|----------|
| **Simple** | 6 lines | 0 lines* | 18 lines |
| **Medium** | 15 lines | 0 lines* | 106 lines |

\* Auto-discovery requires only command: `mirror_bridge_auto src/ --module name`

## Running the Benchmarks

### Prerequisites

```bash
# Build benchmark Docker image (one-time, ~30-60 min)
docker build -f Dockerfile.benchmarks -t mirror_bridge:benchmarks .
```

### Execute Full Suite

```bash
./run_benchmarks.sh
```

This will:
1. Build all binding modules (Mirror Bridge manual, Mirror Bridge auto-discovery, pybind11)
2. Measure compile times for simple and medium projects
3. Run runtime performance tests
4. Generate comparison tables

### Results Location

- **Console output**: Formatted tables with speedup ratios
- **Detailed data**: `benchmarks/runtime/runtime_results.json`

## Expected Results

Based on Mirror Bridge's architecture:

### Compile-Time Expectations

Mirror Bridge should show **2-5x faster** compilation than pybind11 because:
- Template metaprogramming is simpler (reflection vs manual template expansion)
- Less header bloat (single-header vs pybind11's extensive template library)
- Compile-time code generation reduces work for later compilation stages

### Runtime Expectations

Mirror Bridge should show **comparable or slightly better** runtime performance because:
- Both generate similar Python C API calls under the hood
- Mirror Bridge has less template indirection
- Potential gains from simpler generated code

### Developer Experience

Mirror Bridge provides:
- **Manual binding**: 3-7x less code than pybind11
- **Auto-discovery**: Infinite reduction (0 lines vs 18-106 lines)

## Benchmark Implementation

### File Structure

```
benchmarks/
├── run_all_benchmarks.sh         # Main runner
├── run_benchmarks.sh             # Docker wrapper
├── compile_time/
│   ├── simple/
│   │   ├── simple_class.hpp      # Test class
│   │   ├── mirror_bridge_binding.cpp
│   │   └── pybind11_binding.cpp
│   └── medium/
│       ├── classes.hpp           # 10 classes, ~50 methods
│       ├── mirror_bridge_binding.cpp
│       └── pybind11_binding.cpp
└── runtime/
    ├── benchmark_class.hpp       # Shared benchmark class
    ├── mirror_bridge_binding.cpp
    ├── pybind11_binding.cpp
    └── run_runtime_benchmarks.py # Python benchmark runner
```

### Key Features

1. **Fair Comparison**: Both implementations use identical optimization flags
2. **Statistical Rigor**: Multiple runs with median reporting
3. **Cache Clearing**: Attempts to drop caches between runs (requires privileges)
4. **Comprehensive Coverage**: Tests all binding approaches and operation types

## Fairness Validation

To ensure fair comparison:

✅ **Optimization flags**: Both use `-O3 -DNDEBUG`
✅ **Compiler**: Same clang version for both
✅ **Python version**: Same Python 3.10 for both
✅ **Iteration counts**: High enough for statistical significance
✅ **Warmup phase**: Both have warmup before measurement
✅ **Median of multiple runs**: Reduces variance

## Future Work

- [ ] Add Boost.Python benchmarks (currently skipped due to libc++ incompatibility)
- [ ] Test on larger codebases (100+ classes)
- [ ] Memory usage profiling
- [ ] Binary size breakdown analysis
- [ ] Cross-platform benchmarks (Linux ARM64, x86_64)

## Notes

- Boost.Python is currently skipped due to incompatibility with libc++ from clang-p2996
- The `/proc/sys/vm/drop_caches` warnings can be ignored (requires root privileges)
- Auto-discovery for medium project may take longer due to template instantiation complexity
- Binary sizes may vary based on optimization level and target architecture
