#!/usr/bin/env python3
"""
Generate visual comparison charts for the blog post.
Creates bar charts showing performance differences.
"""

import json
import sys


def create_ascii_bar_chart(results_py, results_cpp):
    """Create ASCII bar chart comparing Python vs C++ performance."""

    operations = [
        ('Gaussian Blur', 'gaussian_blur'),
        ('Edge Detection', 'edge_detection'),
        ('Histogram Eq.', 'histogram_eq'),
        ('Brightness', 'brightness'),
        ('Initialization', 'initialization'),
    ]

    print("\n" + "=" * 80)
    print("Performance Comparison - Time per Operation")
    print("=" * 80)
    print("\nPython (baseline):")
    print("-" * 80)

    max_time = max(results_py.values())

    for name, key in operations:
        time_val = results_py[key]
        bar_width = int((time_val / max_time) * 60)
        bar = '█' * bar_width
        print(f"{name:20s} {bar} {time_val:.3f}s")

    print("\nC++ (with Mirror Bridge):")
    print("-" * 80)

    for name, key in operations:
        time_val = results_cpp[key]
        bar_width = int((time_val / max_time) * 60)
        bar = '█' * max(1, bar_width)  # At least 1 char
        speedup = results_py[key] / time_val
        print(f"{name:20s} {bar} {time_val:.3f}s ({speedup:.1f}x faster)")

    print("\n" + "=" * 80)

    # Speedup visualization
    print("\nSpeedup Factors (how many times faster C++ is):")
    print("-" * 80)

    speedups = [(name, results_py[key] / results_cpp[key])
                for name, key in operations]
    speedups.sort(key=lambda x: x[1], reverse=True)

    max_speedup = max(s[1] for s in speedups)

    for name, speedup in speedups:
        bar_width = int((speedup / max_speedup) * 60)
        bar = '█' * bar_width
        print(f"{name:20s} {bar} {speedup:.1f}x")

    print("=" * 80)


def create_markdown_table(results_py, results_cpp):
    """Generate markdown table for the blog post."""

    operations = [
        ('Gaussian Blur', 'gaussian_blur'),
        ('Edge Detection', 'edge_detection'),
        ('Histogram Equalization', 'histogram_eq'),
        ('Brightness Adjustment', 'brightness'),
        ('Initialization', 'initialization'),
    ]

    print("\n## Benchmark Results\n")
    print("| Operation | Python | C++ | Speedup |")
    print("|-----------|--------|-----|---------|")

    for name, key in operations:
        py_time = results_py[key]
        cpp_time = results_cpp[key]
        speedup = py_time / cpp_time
        print(f"| {name:<30s} | {py_time:6.3f}s | {cpp_time:6.3f}s | **{speedup:5.1f}x** |")

    # Total
    py_total = sum(results_py.values())
    cpp_total = sum(results_cpp.values())
    total_speedup = py_total / cpp_total

    print(f"| **TOTAL** | **{py_total:6.3f}s** | **{cpp_total:6.3f}s** | **{total_speedup:5.1f}x** |")
    print()


def create_json_for_visualization(results_py, results_cpp, output_file='benchmark_results.json'):
    """Save results in JSON format for external visualization tools."""

    data = {
        "python": results_py,
        "cpp": results_cpp,
        "speedup": {
            key: results_py[key] / results_cpp[key]
            for key in results_py.keys()
        },
        "metadata": {
            "image_size": "512x512",
            "total_pixels": 262144,
            "python_total": sum(results_py.values()),
            "cpp_total": sum(results_cpp.values()),
            "overall_speedup": sum(results_py.values()) / sum(results_cpp.values())
        }
    }

    with open(output_file, 'w') as f:
        json.dump(data, f, indent=2)

    print(f"\n✓ Results saved to {output_file}")
    print("  Use this with Chart.js, D3.js, or other visualization libraries")


def create_hotspot_visualization(results_py):
    """Show where time is being spent (for flame graph context)."""

    print("\n" + "=" * 80)
    print("Time Distribution - Where is Python Spending Time?")
    print("=" * 80)

    total = sum(results_py.values())

    items = sorted(results_py.items(), key=lambda x: x[1], reverse=True)

    print(f"\nTotal time: {total:.3f}s\n")

    cumulative = 0
    for key, time_val in items:
        percentage = (time_val / total) * 100
        cumulative += percentage
        bar_width = int(percentage / 2)  # Scale to fit
        bar = '█' * bar_width

        name = key.replace('_', ' ').title()
        print(f"{name:25s} {bar:50s} {time_val:6.3f}s ({percentage:5.1f}%) [cumulative: {cumulative:5.1f}%]")

    print("\n" + "=" * 80)
    print("💡 Key insight: Gaussian blur accounts for ~80% of execution time!")
    print("   This is the primary target for C++ optimization.")
    print("=" * 80)


if __name__ == '__main__':
    # Example data (replace with actual benchmark results)
    results_py = {
        'gaussian_blur': 6.630,
        'edge_detection': 1.409,
        'histogram_eq': 0.174,
        'brightness': 0.095,
        'initialization': 0.051,
    }

    results_cpp = {
        'gaussian_blur': 0.114,
        'edge_detection': 0.032,
        'histogram_eq': 0.006,
        'brightness': 0.001,
        'initialization': 0.005,
    }

    print("This script visualizes benchmark results for the blog post")
    print("Run the benchmark first, then use the actual results here")
    print("")

    create_ascii_bar_chart(results_py, results_cpp)
    create_hotspot_visualization(results_py)
    create_markdown_table(results_py, results_cpp)
    create_json_for_visualization(results_py, results_cpp)

    print("\n" + "=" * 80)
    print("Visualization tips for blog post:")
    print("=" * 80)
    print("""
1. Flame Graph:
   - Shows call stacks vertically
   - Width = time spent
   - Hover to see function names
   - Red/orange colors = hot spots

2. Bar Charts:
   - Clear visual comparison
   - Side-by-side Python vs C++
   - Speedup bars show relative improvement

3. Tables:
   - Precise numbers for reference
   - Easy to copy/paste
   - Good for reports

4. JSON Export:
   - Use with interactive visualization libraries
   - Chart.js, D3.js, Plotly, etc.
   - Enables custom visualizations
""")
