#!/usr/bin/env python3
"""
Benchmark Calculator: Python vs C++
"""

import time
import math


def benchmark_python():
    """Benchmark pure Python calculator."""
    from calculator_python import Calculator

    print("=" * 70)
    print("🐍 Benchmarking Pure Python Calculator")
    print("=" * 70)

    calc = Calculator()
    times = {}

    # Test 1: Prime checking (CPU intensive)
    start = time.time()
    primes = [calc.is_prime(n) for n in range(1, 50000)]
    times['prime_check'] = time.time() - start

    # Test 2: Fibonacci (iterative)
    start = time.time()
    fibs = [calc.fibonacci(n) for n in range(1, 5000)]
    times['fibonacci'] = time.time() - start

    # Test 3: Factorial
    start = time.time()
    facts = [calc.factorial(n) for n in range(1, 1000)]
    times['factorial'] = time.time() - start

    # Test 4: Trigonometric functions
    start = time.time()
    for i in range(100000):
        x = i * 0.01
        _ = calc.sin(x) + calc.cos(x) + calc.tan(x)
    times['trig_functions'] = time.time() - start

    # Test 5: Logarithms
    start = time.time()
    for i in range(1, 100000):
        _ = calc.log(i) + calc.log10(i)
    times['logarithms'] = time.time() - start

    # Test 6: GCD computations
    start = time.time()
    gcds = [calc.gcd(i, i+17) for i in range(1, 50000)]
    times['gcd'] = time.time() - start

    # Test 7: Array statistics
    numbers = list(range(1, 10001))
    start = time.time()
    for _ in range(100):
        _ = calc.sum_array(numbers)
        _ = calc.average(numbers)
        _ = calc.std_dev(numbers)
    times['statistics'] = time.time() - start

    total = sum(times.values())

    for name, t in times.items():
        print(f"  {name:20s}  {t:.3f}s")

    print(f"  {'TOTAL':20s}  {total:.3f}s")
    print()

    return times


def benchmark_cpp():
    """Benchmark C++ calculator."""
    try:
        import calculator
    except ImportError:
        print("❌ Error: C++ binding not found!")
        print("   Generate bindings with: mirror_bridge_auto . --module calculator")
        return None

    print("=" * 70)
    print("🚀 Benchmarking C++ Calculator (via Mirror Bridge)")
    print("=" * 70)

    calc = calculator.Calculator()
    times = {}

    # Test 1: Prime checking (CPU intensive)
    start = time.time()
    primes = [calc.is_prime(n) for n in range(1, 50000)]
    times['prime_check'] = time.time() - start

    # Test 2: Fibonacci (iterative)
    start = time.time()
    fibs = [calc.fibonacci(n) for n in range(1, 5000)]
    times['fibonacci'] = time.time() - start

    # Test 3: Factorial
    start = time.time()
    facts = [calc.factorial(n) for n in range(1, 1000)]
    times['factorial'] = time.time() - start

    # Test 4: Trigonometric functions
    start = time.time()
    for i in range(100000):
        x = i * 0.01
        _ = calc.sin(x) + calc.cos(x) + calc.tan(x)
    times['trig_functions'] = time.time() - start

    # Test 5: Logarithms
    start = time.time()
    for i in range(1, 100000):
        _ = calc.log(i) + calc.log10(i)
    times['logarithms'] = time.time() - start

    # Test 6: GCD computations
    start = time.time()
    gcds = [calc.gcd(i, i+17) for i in range(1, 50000)]
    times['gcd'] = time.time() - start

    # Test 7: Array statistics
    numbers = list(range(1, 10001))
    start = time.time()
    for _ in range(100):
        _ = calc.sum_array(numbers)
        _ = calc.average(numbers)
        _ = calc.std_dev(numbers)
    times['statistics'] = time.time() - start

    total = sum(times.values())

    for name, t in times.items():
        print(f"  {name:20s}  {t:.3f}s")

    print(f"  {'TOTAL':20s}  {total:.3f}s")
    print()

    return times


def print_comparison(py_times, cpp_times):
    """Print comparison table."""
    print("=" * 70)
    print("📊 Performance Comparison")
    print("=" * 70)
    print()
    print(f"{'Operation':25s} {'Python':>12s} {'C++':>12s} {'Speedup':>10s}")
    print("-" * 70)

    for key in py_times.keys():
        py_time = py_times[key]
        cpp_time = cpp_times[key]
        speedup = py_time / cpp_time if cpp_time > 0 else 0

        print(f"{key:25s} {py_time:11.3f}s {cpp_time:11.3f}s {speedup:9.1f}x")

    py_total = sum(py_times.values())
    cpp_total = sum(cpp_times.values())
    total_speedup = py_total / cpp_total if cpp_total > 0 else 0

    print("-" * 70)
    print(f"{'TOTAL':25s} {py_total:11.3f}s {cpp_total:11.3f}s {total_speedup:9.1f}x")
    print()

    print("=" * 70)
    print("🎉 Results")
    print("=" * 70)
    print()
    print(f"  C++ is {total_speedup:.1f}x faster than Python!")
    print(f"  Total time: {py_total:.2f}s → {cpp_total:.2f}s")
    print(f"  Time saved: {py_total - cpp_total:.2f}s ({100 * (1 - cpp_total/py_total):.1f}%)")
    print()

    # Find top speedups
    speedups = [(k, py_times[k] / cpp_times[k]) for k in py_times.keys()]
    speedups.sort(key=lambda x: x[1], reverse=True)

    print("  🔥 Biggest speedups:")
    for name, speedup in speedups[:3]:
        print(f"     • {name}: {speedup:.1f}x faster")
    print()

    print("  ✨ And this required ZERO lines of binding code!")
    print("     Mirror Bridge auto-generated everything from the C++ header.")
    print()


if __name__ == '__main__':
    print("=" * 70)
    print("  Calculator Performance Benchmark")
    print("  Python vs C++ (with Mirror Bridge)")
    print("=" * 70)
    print()

    py_times = benchmark_python()
    cpp_times = benchmark_cpp()

    if cpp_times:
        print_comparison(py_times, cpp_times)
