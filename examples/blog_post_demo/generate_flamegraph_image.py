#!/usr/bin/env python3
"""
Generate a flame graph visualization showing where Python spends its time.
Uses cProfile for profiling and creates a visual bar chart.
"""

import cProfile
import pstats
import io
from pstats import SortKey
import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
from matplotlib.patches import Rectangle
import numpy as np


def profile_python_code():
    """Profile the Python image processor."""
    from image_processor_pure_python import ImageProcessor

    pr = cProfile.Profile()
    pr.enable()

    # Run the expensive operations
    processor = ImageProcessor(512, 512)
    processor.fill_with_gradient()
    processor.apply_gaussian_blur(radius=2)
    processor.apply_edge_detection()
    processor.apply_histogram_equalization()
    processor.adjust_brightness(1.2)

    pr.disable()

    return pr


def create_flamegraph_visualization(profile_data, output_file='flamegraph.png'):
    """Create a visual flame graph from profile data."""

    # Get stats
    s = io.StringIO()
    ps = pstats.Stats(profile_data, stream=s)
    ps.strip_dirs()
    ps.sort_stats(SortKey.CUMULATIVE)

    # Extract top functions and their times
    stats = ps.stats

    # Filter to our own code (not stdlib)
    our_funcs = []
    for func, (cc, nc, tt, ct, callers) in stats.items():
        filename, line, func_name = func
        if 'image_processor' in filename:
            our_funcs.append({
                'name': func_name,
                'file': filename,
                'time': ct,  # cumulative time
                'calls': nc
            })

    # Sort by time
    our_funcs.sort(key=lambda x: x['time'], reverse=True)

    # Take top 10
    top_funcs = our_funcs[:10]

    if not top_funcs:
        print("⚠ No profile data found")
        return

    # Create visualization
    fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(14, 10))
    fig.suptitle('Python ImageProcessor Performance Profile', fontsize=16, fontweight='bold')

    # Top chart: Time distribution (flame graph style)
    names = [f['name'] for f in top_funcs]
    times = [f['time'] for f in top_funcs]
    total_time = sum(times)

    # Create horizontal bars with gradient colors
    colors = plt.cm.YlOrRd(np.linspace(0.3, 0.9, len(names)))

    y_pos = np.arange(len(names))
    bars = ax1.barh(y_pos, times, color=colors, edgecolor='black', linewidth=1.5)

    ax1.set_yticks(y_pos)
    ax1.set_yticklabels(names, fontsize=11)
    ax1.set_xlabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax1.set_title('Time Spent in Each Function (Hotspots)', fontsize=13, fontweight='bold')
    ax1.grid(axis='x', alpha=0.3, linestyle='--')

    # Add percentage labels
    for i, (bar, time_val) in enumerate(zip(bars, times)):
        percentage = (time_val / total_time) * 100
        ax1.text(time_val + 0.05, bar.get_y() + bar.get_height()/2,
                f'{time_val:.3f}s ({percentage:.1f}%)',
                va='center', fontsize=10, fontweight='bold')

    # Bottom chart: Cumulative percentage (Pareto chart)
    cumulative = np.cumsum(times) / total_time * 100

    ax2.bar(y_pos, [t/total_time*100 for t in times], color=colors, edgecolor='black', linewidth=1.5, alpha=0.7, label='Individual')
    ax2.plot(y_pos, cumulative, 'bo-', linewidth=2, markersize=8, label='Cumulative')
    ax2.axhline(y=80, color='r', linestyle='--', linewidth=2, label='80% threshold')

    ax2.set_xticks(y_pos)
    ax2.set_xticklabels(names, rotation=45, ha='right', fontsize=10)
    ax2.set_ylabel('Percentage of Total Time (%)', fontsize=12, fontweight='bold')
    ax2.set_title('Cumulative Time Distribution (Pareto Analysis)', fontsize=13, fontweight='bold')
    ax2.legend(loc='upper left')
    ax2.grid(axis='y', alpha=0.3, linestyle='--')

    # Add text box with insights
    insight_text = f"""
    Total Time: {total_time:.3f}s
    Top Function: {names[0]}
    Top 3 Account For: {cumulative[min(2, len(cumulative)-1)]:.1f}%
    """

    props = dict(boxstyle='round', facecolor='wheat', alpha=0.8)
    ax2.text(0.98, 0.97, insight_text.strip(), transform=ax2.transAxes,
            fontsize=11, verticalalignment='top', horizontalalignment='right',
            bbox=props, family='monospace')

    plt.tight_layout()
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"✓ Saved flame graph: {output_file}")

    return output_file


def create_simple_hotspot_chart(profile_data, output_file='hotspots.png'):
    """Create a simple, clear chart showing the main bottlenecks."""

    # Get top operations
    operations = {
        'Gaussian Blur': 0.0,
        'Edge Detection': 0.0,
        'Histogram Eq': 0.0,
        'Brightness': 0.0,
        'Fill Gradient': 0.0,
        'Other': 0.0
    }

    s = io.StringIO()
    ps = pstats.Stats(profile_data, stream=s)
    ps.strip_dirs()

    for func, (cc, nc, tt, ct, callers) in ps.stats.items():
        filename, line, func_name = func

        if 'apply_gaussian_blur' in func_name:
            operations['Gaussian Blur'] += ct
        elif 'apply_edge_detection' in func_name:
            operations['Edge Detection'] += ct
        elif 'apply_histogram' in func_name:
            operations['Histogram Eq'] += ct
        elif 'adjust_brightness' in func_name:
            operations['Brightness'] += ct
        elif 'fill_with_gradient' in func_name:
            operations['Fill Gradient'] += ct
        elif 'image_processor' in filename:
            operations['Other'] += ct

    # Remove zero entries
    operations = {k: v for k, v in operations.items() if v > 0.001}

    if not operations:
        print("⚠ No operation data found")
        return

    # Sort by time
    sorted_ops = sorted(operations.items(), key=lambda x: x[1], reverse=True)
    names = [op[0] for op in sorted_ops]
    times = [op[1] for op in sorted_ops]
    total = sum(times)

    # Create pie chart and bar chart
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))
    fig.suptitle('Python ImageProcessor - Performance Hotspots', fontsize=16, fontweight='bold')

    # Pie chart
    colors = ['#ff6b6b', '#4ecdc4', '#45b7d1', '#feca57', '#48dbfb', '#a29bfe']
    explode = [0.1 if i == 0 else 0 for i in range(len(names))]

    wedges, texts, autotexts = ax1.pie(times, labels=names, autopct='%1.1f%%',
                                         colors=colors, explode=explode,
                                         shadow=True, startangle=90,
                                         textprops={'fontsize': 11, 'weight': 'bold'})

    ax1.set_title('Time Distribution', fontsize=13, fontweight='bold')

    # Bar chart with time values
    y_pos = np.arange(len(names))
    bars = ax2.barh(y_pos, times, color=colors, edgecolor='black', linewidth=2)

    ax2.set_yticks(y_pos)
    ax2.set_yticklabels(names, fontsize=12, fontweight='bold')
    ax2.set_xlabel('Time (seconds)', fontsize=12, fontweight='bold')
    ax2.set_title('Absolute Time Spent', fontsize=13, fontweight='bold')
    ax2.grid(axis='x', alpha=0.3, linestyle='--')

    # Add value labels
    for i, (bar, time_val) in enumerate(zip(bars, times)):
        percentage = (time_val / total) * 100
        ax2.text(time_val + 0.02, bar.get_y() + bar.get_height()/2,
                f'{time_val:.3f}s\n({percentage:.1f}%)',
                va='center', fontsize=10, fontweight='bold')

    # Add insight box
    top_bottleneck = names[0]
    top_time = times[0]
    top_pct = (top_time / total) * 100

    insight = f"""
🔥 PRIMARY BOTTLENECK
{top_bottleneck}
{top_time:.3f}s ({top_pct:.1f}%)

This is where C++ optimization
will have the biggest impact!
    """.strip()

    props = dict(boxstyle='round', facecolor='#ffcccc', alpha=0.9, edgecolor='red', linewidth=2)
    ax2.text(0.98, 0.98, insight, transform=ax2.transAxes,
            fontsize=11, verticalalignment='top', horizontalalignment='right',
            bbox=props, family='monospace', weight='bold')

    plt.tight_layout()
    plt.savefig(output_file, dpi=150, bbox_inches='tight')
    print(f"✓ Saved hotspot chart: {output_file}")

    return output_file


if __name__ == '__main__':
    print("=" * 70)
    print("  Generating Flame Graph and Performance Visualizations")
    print("=" * 70)
    print()

    print("[1/3] Profiling Python code...")
    profile_data = profile_python_code()
    print("✓ Profiling complete")
    print()

    print("[2/3] Creating flame graph visualization...")
    create_flamegraph_visualization(profile_data, 'flamegraph.png')
    print()

    print("[3/3] Creating hotspot analysis chart...")
    create_simple_hotspot_chart(profile_data, 'hotspots.png')
    print()

    print("=" * 70)
    print("  Done!")
    print("=" * 70)
    print()
    print("Generated files:")
    print("  • flamegraph.png - Detailed function-level profile")
    print("  • hotspots.png - High-level operation breakdown")
    print()
    print("Use these in your blog post to show WHERE the bottlenecks are!")
