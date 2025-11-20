// High-performance C++ Image Processor
// Optimized with modern C++ features for maximum speed

#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include <numbers>

class ImageProcessor {
private:
    int width_;
    int height_;
    std::vector<float> pixels_;

public:
    // Default constructor
    ImageProcessor() : width_(0), height_(0) {}

    // Constructor
    ImageProcessor(int width, int height)
        : width_(width), height_(height), pixels_(width * height * 3, 0.0f) {}

    // Accessors
    int get_width() const { return width_; }
    int get_height() const { return height_; }

    void fill_with_gradient() {
        for (int y = 0; y < height_; ++y) {
            for (int x = 0; x < width_; ++x) {
                int idx = (y * width_ + x) * 3;
                pixels_[idx + 0] = (static_cast<float>(x) / width_) * 255.0f;   // R
                pixels_[idx + 1] = (static_cast<float>(y) / height_) * 255.0f;  // G
                pixels_[idx + 2] = 128.0f;  // B
            }
        }
    }

    void apply_gaussian_blur(int radius = 2) {
        // Create Gaussian kernel
        int kernel_size = radius * 2 + 1;
        std::vector<float> kernel(kernel_size * kernel_size);
        float sigma = radius / 2.0f;
        float kernel_sum = 0.0f;

        int k_idx = 0;
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                float exp_val = -(x*x + y*y) / (2.0f * sigma * sigma);
                kernel[k_idx] = std::exp(exp_val) / (2.0f * std::numbers::pi_v<float> * sigma * sigma);
                kernel_sum += kernel[k_idx];
                ++k_idx;
            }
        }

        // Normalize kernel
        for (auto& k : kernel) {
            k /= kernel_sum;
        }

        // Apply convolution
        std::vector<float> new_pixels = pixels_;

        for (int py = 0; py < height_; ++py) {
            for (int px = 0; px < width_; ++px) {
                for (int c = 0; c < 3; ++c) {  // RGB channels
                    float value = 0.0f;
                    k_idx = 0;

                    for (int ky = -radius; ky <= radius; ++ky) {
                        for (int kx = -radius; kx <= radius; ++kx) {
                            // Clamp coordinates
                            int sample_x = std::clamp(px + kx, 0, width_ - 1);
                            int sample_y = std::clamp(py + ky, 0, height_ - 1);

                            int pixel_idx = (sample_y * width_ + sample_x) * 3 + c;
                            value += pixels_[pixel_idx] * kernel[k_idx];
                            ++k_idx;
                        }
                    }

                    int out_idx = (py * width_ + px) * 3 + c;
                    new_pixels[out_idx] = value;
                }
            }
        }

        pixels_ = std::move(new_pixels);
    }

    void adjust_brightness(float factor) {
        for (auto& pixel : pixels_) {
            pixel = std::clamp(pixel * factor, 0.0f, 255.0f);
        }
    }

    void apply_edge_detection() {
        static constexpr int sobel_x[] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
        static constexpr int sobel_y[] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};

        std::vector<float> new_pixels(pixels_.size(), 0.0f);

        for (int y = 1; y < height_ - 1; ++y) {
            for (int x = 1; x < width_ - 1; ++x) {
                for (int c = 0; c < 3; ++c) {
                    float gx = 0.0f;
                    float gy = 0.0f;
                    int k_idx = 0;

                    for (int ky = -1; ky <= 1; ++ky) {
                        for (int kx = -1; kx <= 1; ++kx) {
                            int sample_x = x + kx;
                            int sample_y = y + ky;
                            int pixel_idx = (sample_y * width_ + sample_x) * 3 + c;

                            gx += pixels_[pixel_idx] * sobel_x[k_idx];
                            gy += pixels_[pixel_idx] * sobel_y[k_idx];
                            ++k_idx;
                        }
                    }

                    float magnitude = std::sqrt(gx * gx + gy * gy);
                    int out_idx = (y * width_ + x) * 3 + c;
                    new_pixels[out_idx] = std::min(255.0f, magnitude);
                }
            }
        }

        pixels_ = std::move(new_pixels);
    }

    void apply_histogram_equalization() {
        // Apply to each channel separately
        for (int c = 0; c < 3; ++c) {
            std::vector<int> histogram(256, 0);

            // Build histogram
            for (int y = 0; y < height_; ++y) {
                for (int x = 0; x < width_; ++x) {
                    int idx = (y * width_ + x) * 3 + c;
                    int pixel_val = static_cast<int>(pixels_[idx]);
                    if (pixel_val >= 0 && pixel_val < 256) {
                        histogram[pixel_val]++;
                    }
                }
            }

            // Calculate cumulative distribution
            std::vector<int> cdf(256, 0);
            cdf[0] = histogram[0];
            for (int i = 1; i < 256; ++i) {
                cdf[i] = cdf[i-1] + histogram[i];
            }

            // Find minimum non-zero CDF value
            int cdf_min = 0;
            for (int i = 0; i < 256; ++i) {
                if (cdf[i] > 0) {
                    cdf_min = cdf[i];
                    break;
                }
            }

            int total_pixels = width_ * height_;

            // Apply equalization
            if (cdf_min < total_pixels) {  // Avoid division by zero
                for (int y = 0; y < height_; ++y) {
                    for (int x = 0; x < width_; ++x) {
                        int idx = (y * width_ + x) * 3 + c;
                        int pixel_val = static_cast<int>(pixels_[idx]);
                        if (pixel_val >= 0 && pixel_val < 256) {
                            pixels_[idx] = static_cast<float>((cdf[pixel_val] - cdf_min) * 255) / (total_pixels - cdf_min);
                        }
                    }
                }
            }
        }
    }
};
