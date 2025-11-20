#!/usr/bin/env python3
"""
Pure Python Image Processor - Demonstrating Performance Bottlenecks
This implementation intentionally uses pure Python for CPU-intensive operations
to show the performance gap that Mirror Bridge can help solve.
"""

import time
import math


class ImageProcessor:
    """Process images with various filters and transformations."""

    def __init__(self, width, height):
        self.width = width
        self.height = height
        # Simulate an image as a flat list of RGB pixels
        self.pixels = [0.0] * (width * height * 3)

    def apply_gaussian_blur(self, radius=2):
        """Apply Gaussian blur filter - very CPU intensive."""
        # Create Gaussian kernel
        kernel_size = radius * 2 + 1
        kernel = []
        sigma = radius / 2.0

        for y in range(-radius, radius + 1):
            for x in range(-radius, radius + 1):
                exp_val = -(x*x + y*y) / (2 * sigma * sigma)
                kernel.append(math.exp(exp_val) / (2 * math.pi * sigma * sigma))

        # Normalize kernel
        kernel_sum = sum(kernel)
        kernel = [k / kernel_sum for k in kernel]

        # Apply convolution
        new_pixels = self.pixels.copy()

        for py in range(self.height):
            for px in range(self.width):
                for c in range(3):  # RGB channels
                    value = 0.0
                    k_idx = 0

                    for ky in range(-radius, radius + 1):
                        for kx in range(-radius, radius + 1):
                            # Clamp coordinates
                            sample_x = max(0, min(self.width - 1, px + kx))
                            sample_y = max(0, min(self.height - 1, py + ky))

                            pixel_idx = (sample_y * self.width + sample_x) * 3 + c
                            value += self.pixels[pixel_idx] * kernel[k_idx]
                            k_idx += 1

                    out_idx = (py * self.width + px) * 3 + c
                    new_pixels[out_idx] = value

        self.pixels = new_pixels

    def adjust_brightness(self, factor):
        """Adjust image brightness by a factor."""
        for i in range(len(self.pixels)):
            self.pixels[i] = min(255.0, max(0.0, self.pixels[i] * factor))

    def apply_edge_detection(self):
        """Apply Sobel edge detection filter."""
        sobel_x = [-1, 0, 1, -2, 0, 2, -1, 0, 1]
        sobel_y = [-1, -2, -1, 0, 0, 0, 1, 2, 1]

        new_pixels = [0.0] * len(self.pixels)

        for y in range(1, self.height - 1):
            for x in range(1, self.width - 1):
                for c in range(3):
                    gx = 0.0
                    gy = 0.0
                    k_idx = 0

                    for ky in range(-1, 2):
                        for kx in range(-1, 2):
                            sample_x = x + kx
                            sample_y = y + ky
                            pixel_idx = (sample_y * self.width + sample_x) * 3 + c

                            gx += self.pixels[pixel_idx] * sobel_x[k_idx]
                            gy += self.pixels[pixel_idx] * sobel_y[k_idx]
                            k_idx += 1

                    magnitude = math.sqrt(gx * gx + gy * gy)
                    out_idx = (y * self.width + x) * 3 + c
                    new_pixels[out_idx] = min(255.0, magnitude)

        self.pixels = new_pixels

    def apply_histogram_equalization(self):
        """Enhance contrast using histogram equalization."""
        # Calculate histogram for each channel
        for c in range(3):
            histogram = [0] * 256

            # Build histogram
            for y in range(self.height):
                for x in range(self.width):
                    idx = (y * self.width + x) * 3 + c
                    pixel_val = int(self.pixels[idx])
                    if 0 <= pixel_val < 256:
                        histogram[pixel_val] += 1

            # Calculate cumulative distribution
            cdf = [0] * 256
            cdf[0] = histogram[0]
            for i in range(1, 256):
                cdf[i] = cdf[i-1] + histogram[i]

            # Normalize
            cdf_min = min(val for val in cdf if val > 0) if any(val > 0 for val in cdf) else 0
            total_pixels = self.width * self.height

            # Apply equalization
            if cdf_min < total_pixels:  # Avoid division by zero
                for y in range(self.height):
                    for x in range(self.width):
                        idx = (y * self.width + x) * 3 + c
                        pixel_val = int(self.pixels[idx])
                        if 0 <= pixel_val < 256:
                            self.pixels[idx] = ((cdf[pixel_val] - cdf_min) * 255) / (total_pixels - cdf_min)

    def fill_with_gradient(self):
        """Initialize with a test gradient pattern."""
        for y in range(self.height):
            for x in range(self.width):
                idx = (y * self.width + x) * 3
                # Create RGB gradient
                self.pixels[idx + 0] = (x / self.width) * 255  # R
                self.pixels[idx + 1] = (y / self.height) * 255  # G
                self.pixels[idx + 2] = 128  # B


def run_benchmark():
    """Run comprehensive benchmark of image processing operations."""
    print("=" * 60)
    print("Pure Python Image Processor Benchmark")
    print("=" * 60)

    # Test with a moderately sized image
    width, height = 512, 512
    print(f"\nImage size: {width}x{height} ({width * height:,} pixels)")

    results = {}

    # Initialization
    print("\n1. Creating image and filling with test pattern...")
    start = time.perf_counter()
    processor = ImageProcessor(width, height)
    processor.fill_with_gradient()
    elapsed = time.perf_counter() - start
    results['initialization'] = elapsed
    print(f"   Time: {elapsed:.3f}s")

    # Gaussian blur
    print("\n2. Applying Gaussian blur (radius=2)...")
    start = time.perf_counter()
    processor.apply_gaussian_blur(radius=2)
    elapsed = time.perf_counter() - start
    results['gaussian_blur'] = elapsed
    print(f"   Time: {elapsed:.3f}s")

    # Brightness adjustment
    print("\n3. Adjusting brightness...")
    start = time.perf_counter()
    processor.adjust_brightness(1.2)
    elapsed = time.perf_counter() - start
    results['brightness'] = elapsed
    print(f"   Time: {elapsed:.3f}s")

    # Edge detection
    print("\n4. Applying edge detection...")
    start = time.perf_counter()
    processor.apply_edge_detection()
    elapsed = time.perf_counter() - start
    results['edge_detection'] = elapsed
    print(f"   Time: {elapsed:.3f}s")

    # Histogram equalization
    print("\n5. Applying histogram equalization...")
    processor.fill_with_gradient()  # Reset
    start = time.perf_counter()
    processor.apply_histogram_equalization()
    elapsed = time.perf_counter() - start
    results['histogram_equalization'] = elapsed
    print(f"   Time: {elapsed:.3f}s")

    # Total time
    total_time = sum(results.values())

    print("\n" + "=" * 60)
    print("Summary")
    print("=" * 60)
    print(f"Total time: {total_time:.3f}s")
    print("\nTime breakdown:")
    for op, time_val in sorted(results.items(), key=lambda x: x[1], reverse=True):
        percentage = (time_val / total_time) * 100
        print(f"  {op:30s} {time_val:7.3f}s ({percentage:5.1f}%)")

    print("\n🐌 Python is slow for CPU-intensive image processing!")
    print("   Let's see how much faster C++ can be...")

    return results


if __name__ == '__main__':
    results = run_benchmark()
