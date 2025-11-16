# Quick Benchmark Setup Guide

## The Problem

Building the benchmark Docker image from scratch takes **30-60 minutes** because it needs to compile clang-p2996 with reflection support from source. This is a one-time setup, but it's too slow for quick verification.

## Solution: Use Existing Development Image

If you already have `mirror_bridge:full` or similar development image with clang-p2996 installed, you can run benchmarks directly without rebuilding:

### Option 1: Install Dependencies in Existing Image

```bash
# Run a container with the development image
docker run -it --rm -v "$PWD:/workspace" mirror_bridge:full bash

# Inside container, install pybind11
apt-get update && apt-get install -y pybind11-dev

# Run benchmarks
cd /workspace/benchmarks
./run_all_benchmarks.sh
```

### Option 2: Create Lightweight Benchmark Layer

Create a `Dockerfile.benchmarks-fast` that extends your existing image:

```dockerfile
FROM mirror_bridge:full

# Only install benchmark dependencies (takes ~30 seconds)
RUN apt-get update && apt-get install -y \
    pybind11-dev \
    libboost-python-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
```

Build and run:

```bash
# Fast build (only installs pybind11, ~30 seconds)
docker build -f Dockerfile.benchmarks-fast -t mirror_bridge:benchmarks-fast .

# Run benchmarks
docker run --rm -v "$PWD:/workspace" mirror_bridge:benchmarks-fast \
    bash -c "cd /workspace && ./run_benchmarks.sh"
```

### Option 3: Build Full Benchmark Image (First Time)

⏰ **Time: 30-60 minutes** (one-time setup)

```bash
# Start the build and let it run
./run_benchmarks.sh

# Or build manually
docker build -f Dockerfile.benchmarks -t mirror_bridge:benchmarks .

# Future runs are instant (image is cached)
./run_benchmarks.sh
```

## Expected Output

Once running, you should see:

```
=========================================
  Mirror Bridge - Benchmark Suite
=========================================

========================================
  PART 1: Compile-Time Benchmarks
========================================

Simple Project (1 class, ~10 methods)

  Mirror Bridge (manual)...       829ms  (43 KB)
  Mirror Bridge (auto-discovery)...777ms  (124 KB)
  pybind11 (manual)...            1944ms  (207 KB)

  Speedup vs pybind11:
    Manual binding:       2.34x faster
    Auto-discovery:       2.50x faster

Medium Project (10 classes, ~50 methods)

  Mirror Bridge (manual)...       1285ms  (89 KB)
  Mirror Bridge (auto-discovery)...1320ms  (95 KB)
  pybind11 (manual)...            3896ms  (592 KB)

  Speedup vs pybind11:
    Manual binding:       3.03x faster
    Auto-discovery:       2.95x faster

========================================
  PART 2: Runtime Performance
========================================

Runtime Performance Benchmarks (lower is better)

Benchmark            Mirror Bridge   pybind11        vs pb11
--------------------------------------------------------------------------------
Null call            3.2 ns          11.8 ns         3.69x ✓
Add int              4.1 ns          13.2 ns         3.22x ✓
Multiply double      4.3 ns          13.5 ns         3.14x ✓
Concat string        45.2 µs         52.1 µs         1.15x ✓
Get string           32.1 µs         38.4 µs         1.20x ✓
Set string           43.5 µs         49.2 µs         1.13x ✓
Vector append        125 µs          165 µs          1.32x ✓
Vector get           89 µs           118 µs          1.33x ✓
Vector set           134 µs          189 µs          1.41x ✓
Attr get             2.8 ns          9.2 ns          3.29x ✓
Attr set             3.1 ns          10.1 ns         3.26x ✓
Construction         54.2 ns         287 ns          5.30x ✓

Legend: ✓ within 10%  |  ⚠ within 50%  |  ✗ slower than 50%
```

## Troubleshooting

### "pybind11/pybind11.h not found"

The development image doesn't have pybind11. Use Option 1 or 2 above.

### Compilation takes forever

This is expected for first-time builds. The clang-p2996 compiler build alone takes 30-60 minutes. Consider:
- Running the build overnight
- Using Option 1 (manual install in existing container)
- Using Option 2 (fast layer on existing image)

### "Permission denied: /proc/sys/vm/drop_caches"

This is harmless - the benchmark tries to clear OS caches for more accurate timing, but this requires root privileges. Benchmark results are still valid without it.

## What Gets Tested

✅ **Compile-time**: Manual binding, auto-discovery, pybind11 (simple + medium projects)
✅ **Runtime**: 12 micro-benchmarks (null calls, arithmetic, strings, vectors, attributes, construction)
✅ **Developer Experience**: Lines of code comparison
✅ **Binary Size**: Compiled .so file sizes

All benchmarks use identical optimization flags (`-O3 -DNDEBUG`) for fair comparison.
