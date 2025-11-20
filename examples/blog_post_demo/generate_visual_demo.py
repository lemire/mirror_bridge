#!/usr/bin/env python3
"""
Generate visual demonstration images showing the image transformations.
Creates actual PNG images showing before/after effects.
"""

import sys
from PIL import Image, ImageDraw, ImageFont
import numpy as np

# Try to import the C++ version, fall back to Python
try:
    import image_processor
    print("✓ Using C++ ImageProcessor (via Mirror Bridge)")
    use_cpp = True
except ImportError:
    from image_processor_pure_python import ImageProcessor as PyImageProcessor
    print("⚠ Using pure Python ImageProcessor (C++ binding not found)")
    use_cpp = False


def pixels_to_image(pixels, width, height):
    """Convert flat pixel array to PIL Image."""
    # Reshape and convert to uint8
    arr = np.array(pixels, dtype=np.float32).reshape((height, width, 3))
    arr = np.clip(arr, 0, 255).astype(np.uint8)
    return Image.fromarray(arr, 'RGB')


def create_demo_image(width=512, height=512):
    """Create a demo image with interesting features."""
    if use_cpp:
        processor = image_processor.ImageProcessor(width, height)
        processor.fill_with_gradient()

        # Get pixels as a list
        # We need to extract pixels from the C++ object
        # For now, just create using Python and pass to C++
        img = Image.new('RGB', (width, height))
        draw = ImageDraw.Draw(img)

        # Create gradient background
        for y in range(height):
            for x in range(width):
                r = int((x / width) * 255)
                g = int((y / height) * 255)
                b = 128
                img.putpixel((x, y), (r, g, b))

        # Add some geometric shapes
        draw.rectangle([50, 50, 200, 200], fill=(255, 100, 100), outline=(255, 255, 255), width=3)
        draw.ellipse([300, 100, 450, 250], fill=(100, 100, 255), outline=(255, 255, 255), width=3)
        draw.polygon([(256, 300), (150, 450), (362, 450)], fill=(100, 255, 100), outline=(255, 255, 255), width=3)

        return img
    else:
        # Create using Python
        img = Image.new('RGB', (width, height))
        draw = ImageDraw.Draw(img)

        # Gradient background
        for y in range(height):
            for x in range(width):
                r = int((x / width) * 255)
                g = int((y / height) * 255)
                b = 128
                img.putpixel((x, y), (r, g, b))

        # Shapes
        draw.rectangle([50, 50, 200, 200], fill=(255, 100, 100), outline=(255, 255, 255), width=3)
        draw.ellipse([300, 100, 450, 250], fill=(100, 100, 255), outline=(255, 255, 255), width=3)
        draw.polygon([(256, 300), (150, 450), (362, 450)], fill=(100, 255, 100), outline=(255, 255, 255), width=3)

        return img


def image_to_pixels(img):
    """Convert PIL Image to flat pixel list."""
    arr = np.array(img, dtype=np.float32)
    return arr.flatten().tolist()


def apply_transformations_cpp(input_image):
    """Apply transformations using C++ ImageProcessor."""
    width, height = input_image.size
    pixels = image_to_pixels(input_image)

    # Create processor and set pixels manually
    processor = image_processor.ImageProcessor(width, height)
    # Note: We'd need a set_pixels method in C++ to do this properly
    # For now, let's just demonstrate with a fresh processor

    results = {}

    # Original with gradient
    processor_grad = image_processor.ImageProcessor(width, height)
    processor_grad.fill_with_gradient()
    results['gradient'] = input_image.copy()  # Use the input we created

    # Gaussian blur - we need to convert our image first
    # For demo purposes, let's just show the operations
    results['blur'] = input_image.copy()
    results['edge'] = input_image.copy()
    results['bright'] = input_image.copy()
    results['histogram'] = input_image.copy()

    return results


def apply_transformations_python(input_image):
    """Apply transformations using Python ImageProcessor."""
    from image_processor_pure_python import ImageProcessor as PyImageProcessor

    width, height = input_image.size
    pixels = image_to_pixels(input_image)

    results = {}

    # Original
    processor = PyImageProcessor(width, height)
    for i, val in enumerate(pixels):
        processor.pixels[i] = val
    results['original'] = pixels_to_image(processor.pixels, width, height)

    # Gaussian Blur
    processor_blur = PyImageProcessor(width, height)
    for i, val in enumerate(pixels):
        processor_blur.pixels[i] = val
    processor_blur.apply_gaussian_blur(radius=3)
    results['blur'] = pixels_to_image(processor_blur.pixels, width, height)

    # Edge Detection
    processor_edge = PyImageProcessor(width, height)
    for i, val in enumerate(pixels):
        processor_edge.pixels[i] = val
    processor_edge.apply_edge_detection()
    results['edge'] = pixels_to_image(processor_edge.pixels, width, height)

    # Brightness
    processor_bright = PyImageProcessor(width, height)
    for i, val in enumerate(pixels):
        processor_bright.pixels[i] = val
    processor_bright.adjust_brightness(1.5)
    results['bright'] = pixels_to_image(processor_bright.pixels, width, height)

    # Histogram Equalization
    processor_hist = PyImageProcessor(width, height)
    for i, val in enumerate(pixels):
        processor_hist.pixels[i] = val
    processor_hist.apply_histogram_equalization()
    results['histogram'] = pixels_to_image(processor_hist.pixels, width, height)

    return results


def create_comparison_image(results, output_file='transformations.png'):
    """Create a side-by-side comparison image."""
    # Create a grid: 2 rows x 3 columns
    ops = ['original', 'blur', 'edge', 'bright', 'histogram']

    if 'original' not in results:
        results['original'] = results.get('gradient', list(results.values())[0])

    # Get dimensions from first image
    first_img = results[ops[0]]
    img_width, img_height = first_img.size

    # Create canvas: 3 columns, 2 rows
    padding = 10
    label_height = 40
    cols = 3
    rows = 2

    canvas_width = cols * img_width + (cols + 1) * padding
    canvas_height = rows * (img_height + label_height) + (rows + 1) * padding

    canvas = Image.new('RGB', (canvas_width, canvas_height), color=(40, 40, 40))
    draw = ImageDraw.Draw(canvas)

    labels = {
        'original': 'Original Image',
        'blur': 'Gaussian Blur',
        'edge': 'Edge Detection',
        'bright': 'Brightness +50%',
        'histogram': 'Histogram Equalization'
    }

    for idx, op in enumerate(ops):
        if op not in results:
            continue

        row = idx // cols
        col = idx % cols

        x = padding + col * (img_width + padding)
        y = padding + row * (img_height + label_height + padding)

        # Paste image
        canvas.paste(results[op], (x, y + label_height))

        # Draw label
        label = labels.get(op, op.title())
        # Try to use a font, fall back to default
        try:
            font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 20)
        except:
            font = ImageFont.load_default()

        # Get text bbox
        bbox = draw.textbbox((x, y), label, font=font)
        text_width = bbox[2] - bbox[0]

        # Center text
        text_x = x + (img_width - text_width) // 2
        draw.text((text_x, y + 10), label, fill=(255, 255, 255), font=font)

    canvas.save(output_file)
    print(f"✓ Saved comparison image: {output_file}")
    return output_file


if __name__ == '__main__':
    print("=" * 70)
    print("  Generating Visual Demonstration")
    print("=" * 70)
    print()

    print("[1/3] Creating demo image...")
    demo_img = create_demo_image(512, 512)
    demo_img.save('demo_original.png')
    print("✓ Saved: demo_original.png")
    print()

    print("[2/3] Applying transformations...")
    if use_cpp:
        # For C++, we'll use Python for now since we need pixel access methods
        print("  Note: Using Python for transformations (C++ needs pixel getters/setters)")
        results = apply_transformations_python(demo_img)
    else:
        results = apply_transformations_python(demo_img)

    print(f"✓ Applied {len(results)} transformations")
    print()

    print("[3/3] Creating comparison image...")
    create_comparison_image(results, 'transformations.png')
    print()

    print("=" * 70)
    print("  Done!")
    print("=" * 70)
    print()
    print("Generated files:")
    print("  • demo_original.png - Original demo image")
    print("  • transformations.png - Side-by-side comparison")
    print()
    print("Use these images in your blog post to show what the")
    print("ImageProcessor does visually!")
