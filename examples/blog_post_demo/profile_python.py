#!/usr/bin/env python3
"""
Profile the pure Python image processor to identify bottlenecks.
Generates flame graph data and detailed profiling statistics.
"""

import cProfile
import pstats
import io
from image_processor_pure_python import ImageProcessor


def profile_operations():
    """Profile each operation individually."""
    width, height = 512, 512

    # Profile Gaussian blur (the slowest operation)
    print("Profiling Gaussian blur...")
    pr = cProfile.Profile()
    pr.enable()

    processor = ImageProcessor(width, height)
    processor.fill_with_gradient()
    processor.apply_gaussian_blur(radius=2)

    pr.disable()

    # Print stats
    s = io.StringIO()
    ps = pstats.Stats(pr, stream=s).sort_stats('cumulative')
    ps.print_stats(20)  # Top 20 functions

    print("\n" + "=" * 80)
    print("Top 20 Functions by Cumulative Time")
    print("=" * 80)
    print(s.getvalue())

    # Save detailed stats
    ps.dump_stats('python_profile.prof')
    print("\nProfile data saved to: python_profile.prof")
    print("View with: python -m pstats python_profile.prof")


def generate_flame_graph_data():
    """Generate data for flame graph visualization."""
    import sys

    print("\n" + "=" * 80)
    print("Flame Graph Analysis")
    print("=" * 80)

    try:
        # Try to use py-spy if available for better visualization
        import subprocess
        print("\nTo generate a flame graph, run:")
        print("  pip install py-spy")
        print("  py-spy record -o profile.svg -- python image_processor_pure_python.py")
        print("\nOr use built-in profiler:")
        print("  python -m cProfile -o output.prof image_processor_pure_python.py")
        print("  gprof2dot -f pstats output.prof | dot -Tpng -o profile.png")
    except:
        pass

    # Simple visualization of where time is spent
    width, height = 512, 512
    processor = ImageProcessor(width, height)
    processor.fill_with_gradient()

    operations = {
        'Gaussian Blur': lambda: processor.apply_gaussian_blur(radius=2),
        'Brightness': lambda: processor.adjust_brightness(1.2),
        'Edge Detection': lambda: processor.apply_edge_detection(),
        'Histogram Eq.': lambda: processor.apply_histogram_equalization(),
    }

    print("\nTime distribution (approximate):")
    print("-" * 80)

    import time
    total = 0
    times = {}

    for name, func in operations.items():
        processor.fill_with_gradient()  # Reset
        start = time.perf_counter()
        func()
        elapsed = time.perf_counter() - start
        times[name] = elapsed
        total += elapsed

    for name, elapsed in sorted(times.items(), key=lambda x: x[1], reverse=True):
        bar_width = int((elapsed / total) * 50)
        percentage = (elapsed / total) * 100
        bar = '█' * bar_width
        print(f"{name:20s} {bar:50s} {percentage:5.1f}% ({elapsed:.3f}s)")

    print("-" * 80)
    print(f"{'TOTAL':20s} {'█' * 50:50s} 100.0% ({total:.3f}s)")

    print("\n💡 Key bottlenecks:")
    print("   - Nested loops over image pixels")
    print("   - Python function call overhead")
    print("   - Lack of vectorization")
    print("   - GIL preventing multi-threading")


if __name__ == '__main__':
    profile_operations()
    generate_flame_graph_data()
