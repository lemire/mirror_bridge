#!/usr/bin/env python3
"""
Benchmark: Python vs C++ for the hot path (find_pattern)

This shows the power of surgical optimization:
- Keep most code in Python (easy to read/maintain)
- Replace JUST the hot function with C++
- Get massive speedup with minimal effort
"""

import time
from log_parser import LogParser, generate_fake_logs

# Try to import C++ version
try:
    import pattern_matcher
    HAS_CPP = True
    print("✓ C++ PatternMatcher loaded")
except ImportError:
    HAS_CPP = False
    print("⚠ C++ PatternMatcher not found (run ./run_auto_discovery.sh)")

print()


def benchmark_python(logs, patterns):
    """Benchmark pure Python implementation."""
    print("=" * 70)
    print("🐍 Benchmarking Pure Python")
    print("=" * 70)

    parser = LogParser()
    times = {}

    # Single pattern search
    print("\nSingle pattern search (find_pattern):")
    start = time.time()
    count = parser.find_pattern(logs, "ERROR")
    times['single_pattern'] = time.time() - start
    print(f"  'ERROR': {count:,} matches in {times['single_pattern']:.3f}s")

    # Multiple pattern search
    print("\nMultiple pattern search:")
    start = time.time()
    results = parser.search_multiple_patterns(logs, patterns)
    times['multi_pattern'] = time.time() - start

    print(f"  Searched {len(patterns)} patterns:")
    for pattern, count in results.items():
        print(f"    {pattern}: {count:,}")
    print(f"  Total time: {times['multi_pattern']:.3f}s")

    total = sum(times.values())
    print(f"\n  TOTAL: {total:.3f}s")
    print()

    return times, results


def benchmark_cpp(logs, patterns):
    """Benchmark C++ implementation - just the hot function replaced."""
    if not HAS_CPP:
        return None, None

    print("=" * 70)
    print("🚀 Benchmarking C++ (JUST find_pattern replaced)")
    print("=" * 70)

    matcher = pattern_matcher.PatternMatcher()
    times = {}

    # Single pattern search
    print("\nSingle pattern search (find_pattern):")
    start = time.time()
    count = matcher.find_pattern(logs, "ERROR")
    times['single_pattern'] = time.time() - start
    print(f"  'ERROR': {count:,} matches in {times['single_pattern']:.3f}s")

    # Multiple pattern search
    print("\nMultiple pattern search:")
    start = time.time()
    counts = matcher.search_multiple_patterns(logs, patterns)
    times['multi_pattern'] = time.time() - start

    print(f"  Searched {len(patterns)} patterns:")
    for pattern, count in zip(patterns, counts):
        print(f"    {pattern}: {count:,}")
    print(f"  Total time: {times['multi_pattern']:.3f}s")

    total = sum(times.values())
    print(f"\n  TOTAL: {total:.3f}s")
    print()

    results = {p: c for p, c in zip(patterns, counts)}
    return times, results


def print_comparison(py_times, cpp_times):
    """Print side-by-side comparison."""
    print("=" * 70)
    print("📊 Performance Comparison - Python vs C++")
    print("=" * 70)
    print()

    print(f"{'Operation':<30} {'Python':>12} {'C++':>12} {'Speedup':>10}")
    print("-" * 70)

    for key in py_times.keys():
        py_time = py_times[key]
        cpp_time = cpp_times[key]
        speedup = py_time / cpp_time if cpp_time > 0 else 0

        op_name = key.replace('_', ' ').title()
        print(f"{op_name:<30} {py_time:11.3f}s {cpp_time:11.3f}s {speedup:9.1f}x")

    py_total = sum(py_times.values())
    cpp_total = sum(cpp_times.values())
    total_speedup = py_total / cpp_total if cpp_total > 0 else 0

    print("-" * 70)
    print(f"{'TOTAL':<30} {py_total:11.3f}s {cpp_total:11.3f}s {total_speedup:9.1f}x")
    print()

    print("=" * 70)
    print("🎉 Results")
    print("=" * 70)
    print()
    print(f"  C++ is {total_speedup:.1f}x faster!")
    print(f"  Time: {py_total:.2f}s → {cpp_total:.2f}s")
    print(f"  Saved: {py_total - cpp_total:.2f}s ({100 * (1 - cpp_total/py_total):.1f}%)")
    print()
    print("  💡 Key insight:")
    print("     We replaced JUST ONE FUNCTION (find_pattern) with C++")
    print("     The rest of the code is still Python!")
    print()
    print("  ✨ And this required ZERO lines of binding code!")
    print("     Mirror Bridge auto-generated everything.")
    print()


if __name__ == '__main__':
    print("=" * 70)
    print("  Log Parser Benchmark: Python vs C++")
    print("  (Surgical Optimization Demo)")
    print("=" * 70)
    print()

    # Generate test data
    print("Generating 50 MB of log data...")
    logs = generate_fake_logs(50)
    print(f"  Size: {len(logs):,} characters ({len(logs)/1024/1024:.1f} MB)")
    print(f"  Lines: {logs.count(chr(10)):,}")
    print()

    # Patterns to search for
    patterns = ["ERROR", "WARNING", "INFO", "DEBUG", "timeout", "failed", "success"]
    print(f"Searching for {len(patterns)} patterns: {', '.join(patterns)}")
    print()

    # Benchmark Python
    py_times, py_results = benchmark_python(logs, patterns)

    # Benchmark C++
    cpp_times, cpp_results = benchmark_cpp(logs, patterns)

    # Compare
    if cpp_times:
        # Verify results match
        print("Verifying results match...")
        match = True
        for pattern in patterns:
            if py_results[pattern] != cpp_results[pattern]:
                print(f"  ⚠️  Mismatch: {pattern} - Python={py_results[pattern]}, C++={cpp_results[pattern]}")
                match = False

        if match:
            print("  ✓ All results match perfectly!")
            print()

        print_comparison(py_times, cpp_times)
    else:
        print("Skipping comparison (C++ version not available)")
        print("Run ./run_auto_discovery.sh to generate C++ bindings")
