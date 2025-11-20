#!/usr/bin/env python3
"""
Benchmark comparison: Pure Python vs C++ with Mirror Bridge bindings
Shows the dramatic performance improvement from using C++ for CPU-intensive operations.
"""

import time
import sys


def benchmark_python():
    """Benchmark the pure Python implementation."""
    from image_processor_pure_python import ImageProcessor

    print("=" * 70)
    print("🐍 Benchmarking Pure Python Implementation")
    print("=" * 70)

    width, height = 512, 512
    results = {}

    # Initialization
    start = time.perf_counter()
    processor = ImageProcessor(width, height)
    processor.fill_with_gradient()
    elapsed = time.perf_counter() - start
    results['initialization'] = elapsed
    print(f"  Initialization:           {elapsed:7.3f}s")

    # Gaussian blur
    start = time.perf_counter()
    processor.apply_gaussian_blur(radius=2)
    elapsed = time.perf_counter() - start
    results['gaussian_blur'] = elapsed
    print(f"  Gaussian Blur:            {elapsed:7.3f}s")

    # Brightness
    start = time.perf_counter()
    processor.adjust_brightness(1.2)
    elapsed = time.perf_counter() - start
    results['brightness'] = elapsed
    print(f"  Brightness Adjustment:    {elapsed:7.3f}s")

    # Edge detection
    start = time.perf_counter()
    processor.apply_edge_detection()
    elapsed = time.perf_counter() - start
    results['edge_detection'] = elapsed
    print(f"  Edge Detection:           {elapsed:7.3f}s")

    # Histogram equalization
    processor.fill_with_gradient()
    start = time.perf_counter()
    processor.apply_histogram_equalization()
    elapsed = time.perf_counter() - start
    results['histogram_eq'] = elapsed
    print(f"  Histogram Equalization:   {elapsed:7.3f}s")

    total = sum(results.values())
    print(f"  {'TOTAL:':24s}  {total:7.3f}s")

    return results, total


def benchmark_cpp():
    """Benchmark the C++ implementation with Mirror Bridge bindings."""
    try:
        import image_processor
    except ImportError:
        print("\n❌ Error: C++ binding not found!")
        print("   Run ./run_auto_discovery.sh first to generate bindings")
        sys.exit(1)

    print("\n" + "=" * 70)
    print("🚀 Benchmarking C++ Implementation (via Mirror Bridge)")
    print("=" * 70)

    width, height = 512, 512
    results = {}

    # Initialization
    start = time.perf_counter()
    processor = image_processor.ImageProcessor(width, height)
    processor.fill_with_gradient()
    elapsed = time.perf_counter() - start
    results['initialization'] = elapsed
    print(f"  Initialization:           {elapsed:7.3f}s")

    # Gaussian blur
    start = time.perf_counter()
    processor.apply_gaussian_blur(2)
    elapsed = time.perf_counter() - start
    results['gaussian_blur'] = elapsed
    print(f"  Gaussian Blur:            {elapsed:7.3f}s")

    # Brightness
    start = time.perf_counter()
    processor.adjust_brightness(1.2)
    elapsed = time.perf_counter() - start
    results['brightness'] = elapsed
    print(f"  Brightness Adjustment:    {elapsed:7.3f}s")

    # Edge detection
    start = time.perf_counter()
    processor.apply_edge_detection()
    elapsed = time.perf_counter() - start
    results['edge_detection'] = elapsed
    print(f"  Edge Detection:           {elapsed:7.3f}s")

    # Histogram equalization
    processor.fill_with_gradient()
    start = time.perf_counter()
    processor.apply_histogram_equalization()
    elapsed = time.perf_counter() - start
    results['histogram_eq'] = elapsed
    print(f"  Histogram Equalization:   {elapsed:7.3f}s")

    total = sum(results.values())
    print(f"  {'TOTAL:':24s}  {elapsed:7.3f}s")

    return results, total


def print_comparison(py_results, py_total, cpp_results, cpp_total):
    """Print detailed comparison with speedup factors."""
    print("\n" + "=" * 70)
    print("📊 Performance Comparison")
    print("=" * 70)
    print(f"\n{'Operation':<30s} {'Python':>10s} {'C++':>10s} {'Speedup':>10s}")
    print("-" * 70)

    operations = [
        ('Initialization', 'initialization'),
        ('Gaussian Blur', 'gaussian_blur'),
        ('Brightness Adjustment', 'brightness'),
        ('Edge Detection', 'edge_detection'),
        ('Histogram Equalization', 'histogram_eq'),
    ]

    for name, key in operations:
        py_time = py_results[key]
        cpp_time = cpp_results[key]
        speedup = py_time / cpp_time
        print(f"{name:<30s} {py_time:>9.3f}s {cpp_time:>9.3f}s {speedup:>9.1f}x")

    print("-" * 70)
    overall_speedup = py_total / cpp_total
    print(f"{'TOTAL':<30s} {py_total:>9.3f}s {cpp_total:>9.3f}s {overall_speedup:>9.1f}x")

    print("\n" + "=" * 70)
    print("🎉 Results")
    print("=" * 70)
    print(f"\n  C++ is {overall_speedup:.1f}x faster than Python!")
    print(f"  Total speedup: {py_total:.2f}s → {cpp_total:.2f}s")
    print(f"  Time saved: {py_total - cpp_total:.2f}s ({((py_total - cpp_total) / py_total * 100):.1f}%)")

    print("\n  🔥 Biggest speedups:")
    speedups = [(name, py_results[key] / cpp_results[key]) for name, key in operations]
    for name, speedup in sorted(speedups, key=lambda x: x[1], reverse=True)[:3]:
        print(f"     • {name}: {speedup:.1f}x faster")

    print("\n  ✨ And this required ZERO lines of binding code!")
    print("     Mirror Bridge auto-generated everything from the C++ header.")


def main():
    """Run the complete benchmark comparison."""
    print("\n" + "=" * 70)
    print("  Image Processor Performance Benchmark")
    print("  Comparing Pure Python vs C++ (with Mirror Bridge)")
    print("=" * 70)
    print("\n  Image size: 512x512 (262,144 pixels)")
    print("\n")

    # Run Python benchmark
    py_results, py_total = benchmark_python()

    # Run C++ benchmark
    cpp_results, cpp_total = benchmark_cpp()

    # Print comparison
    print_comparison(py_results, py_total, cpp_results, cpp_total)


if __name__ == '__main__':
    main()
