#!/usr/bin/env python3
"""
Benchmark comparing pure Python vs C++ TextAnalyzer implementations.
"""

import time
import sys


def generate_large_text(size_mb=10):
    """Generate a large text file for testing."""
    # Create realistic text with repeated paragraphs
    paragraph = (
        "The quick brown fox jumps over the lazy dog. "
        "This is a sample text used for benchmarking text processing performance. "
        "We need to analyze various properties like word counts, character frequencies, "
        "and perform string transformations. Python is great for productivity, but C++ "
        "excels at raw performance for CPU-intensive operations. Mirror Bridge lets us "
        "combine the best of both worlds with zero binding code! "
    ) * 100  # Repeat to make it larger

    # Calculate how many paragraphs we need
    target_size = size_mb * 1024 * 1024
    current_size = len(paragraph)
    repetitions = target_size // current_size

    return paragraph * repetitions


def benchmark_python(text):
    """Benchmark pure Python implementation."""
    from text_analyzer_python import TextAnalyzer

    print("=" * 70)
    print("🐍 Benchmarking Pure Python Implementation")
    print("=" * 70)

    times = {}

    # Test 1: Create analyzer and set text
    start = time.time()
    analyzer = TextAnalyzer(text)
    times['initialization'] = time.time() - start

    # Test 2: Count characters
    start = time.time()
    char_count = analyzer.count_chars()
    times['count_chars'] = time.time() - start

    # Test 3: Count words
    start = time.time()
    word_count = analyzer.count_words()
    times['count_words'] = time.time() - start

    # Test 4: Count vowels (run multiple times for better measurement)
    start = time.time()
    for _ in range(10):
        vowel_count = analyzer.count_vowels()
    times['count_vowels'] = time.time() - start

    # Test 5: Count consonants (run multiple times)
    start = time.time()
    for _ in range(10):
        consonant_count = analyzer.count_consonants()
    times['count_consonants'] = time.time() - start

    # Test 6: Find most common character (run multiple times)
    start = time.time()
    for _ in range(10):
        most_common = analyzer.most_common_char()
    times['most_common'] = time.time() - start

    # Test 7: Convert to uppercase
    start = time.time()
    upper = analyzer.to_uppercase()
    times['to_uppercase'] = time.time() - start

    # Test 8: Reverse text
    start = time.time()
    reversed_text = analyzer.reverse()
    times['reverse'] = time.time() - start

    total = sum(times.values())

    for name, t in times.items():
        print(f"  {name:20s}  {t:.3f}s")

    print(f"  {'TOTAL':20s}  {total:.3f}s")
    print()

    return times, {
        'chars': char_count,
        'words': word_count,
        'vowels': vowel_count,
        'consonants': consonant_count,
        'most_common': most_common
    }


def benchmark_cpp(text):
    """Benchmark C++ implementation via Mirror Bridge."""
    try:
        import text_analyzer
    except ImportError:
        print("❌ Error: C++ binding not found!")
        print("   Run ./run_auto_discovery.sh first to generate bindings")
        return None, None

    print("=" * 70)
    print("🚀 Benchmarking C++ Implementation (via Mirror Bridge)")
    print("=" * 70)

    times = {}

    # Test 1: Create analyzer and set text
    start = time.time()
    analyzer = text_analyzer.TextAnalyzer(text)
    times['initialization'] = time.time() - start

    # Test 2: Count characters
    start = time.time()
    char_count = analyzer.count_chars()
    times['count_chars'] = time.time() - start

    # Test 3: Count words
    start = time.time()
    word_count = analyzer.count_words()
    times['count_words'] = time.time() - start

    # Test 4: Count vowels (run multiple times for better measurement)
    start = time.time()
    for _ in range(10):
        vowel_count = analyzer.count_vowels()
    times['count_vowels'] = time.time() - start

    # Test 5: Count consonants (run multiple times)
    start = time.time()
    for _ in range(10):
        consonant_count = analyzer.count_consonants()
    times['count_consonants'] = time.time() - start

    # Test 6: Find most common character (run multiple times)
    start = time.time()
    for _ in range(10):
        most_common = analyzer.most_common_char()
    times['most_common'] = time.time() - start

    # Test 7: Convert to uppercase
    start = time.time()
    upper = analyzer.to_uppercase()
    times['to_uppercase'] = time.time() - start

    # Test 8: Reverse text
    start = time.time()
    reversed_text = analyzer.reverse()
    times['reverse'] = time.time() - start

    total = sum(times.values())

    for name, t in times.items():
        print(f"  {name:20s}  {t:.3f}s")

    print(f"  {'TOTAL':20s}  {total:.3f}s")
    print()

    return times, {
        'chars': char_count,
        'words': word_count,
        'vowels': vowel_count,
        'consonants': consonant_count,
        'most_common': most_common
    }


def print_comparison(py_times, cpp_times):
    """Print side-by-side comparison."""
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
    print(f"  Total speedup: {py_total:.2f}s → {cpp_total:.2f}s")
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
    print("  Text Analyzer Performance Benchmark")
    print("  Comparing Pure Python vs C++ (with Mirror Bridge)")
    print("=" * 70)
    print()

    # Generate test data
    print("Generating 10 MB of text data...")
    text = generate_large_text(10)
    print(f"  Text size: {len(text):,} characters ({len(text) / 1024 / 1024:.1f} MB)")
    print(f"  Approx words: {len(text.split()):,}")
    print()

    # Run benchmarks
    py_times, py_results = benchmark_python(text)
    cpp_times, cpp_results = benchmark_cpp(text)

    if cpp_times is not None:
        # Verify results match
        print("Verifying results match...")
        for key in ['chars', 'words', 'vowels', 'consonants']:
            if py_results[key] != cpp_results[key]:
                print(f"  ⚠️  Mismatch in {key}: Python={py_results[key]}, C++={cpp_results[key]}")
            else:
                print(f"  ✓ {key}: {py_results[key]:,}")
        print()

        # Print comparison
        print_comparison(py_times, cpp_times)
