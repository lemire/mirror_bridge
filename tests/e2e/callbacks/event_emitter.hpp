// Event emitter example for testing std::function callback support
// Demonstrates passing Python callables to C++ code

#pragma once
#include <functional>
#include <string>
#include <vector>

// Simple event emitter that stores and invokes callbacks
class EventEmitter {
public:
    // Callback types
    using DataCallback = std::function<void(int)>;
    using MessageCallback = std::function<void(const std::string&)>;
    using ComputeCallback = std::function<int(int, int)>;

    // Set callbacks
    void on_data(DataCallback cb) {
        data_callback_ = std::move(cb);
    }

    void on_message(MessageCallback cb) {
        message_callback_ = std::move(cb);
    }

    void on_compute(ComputeCallback cb) {
        compute_callback_ = std::move(cb);
    }

    // Emit events (call the callbacks)
    void emit_data(int value) {
        if (data_callback_) {
            data_callback_(value);
        }
    }

    void emit_message(const std::string& msg) {
        if (message_callback_) {
            message_callback_(msg);
        }
    }

    // Use compute callback and return result
    int compute(int a, int b) {
        if (compute_callback_) {
            return compute_callback_(a, b);
        }
        return 0;  // Default if no callback set
    }

    // Check if callbacks are set
    bool has_data_callback() const { return static_cast<bool>(data_callback_); }
    bool has_message_callback() const { return static_cast<bool>(message_callback_); }
    bool has_compute_callback() const { return static_cast<bool>(compute_callback_); }

    // Clear callbacks
    void clear_callbacks() {
        data_callback_ = nullptr;
        message_callback_ = nullptr;
        compute_callback_ = nullptr;
    }

private:
    DataCallback data_callback_;
    MessageCallback message_callback_;
    ComputeCallback compute_callback_;
};

// Processor class that uses callbacks for transformation
class DataProcessor {
public:
    using TransformCallback = std::function<double(double)>;

    void set_transform(TransformCallback cb) {
        transform_ = std::move(cb);
    }

    // Process a list of values using the transform callback
    std::vector<double> process(const std::vector<double>& values) {
        std::vector<double> result;
        result.reserve(values.size());
        for (double v : values) {
            result.push_back(transform_ ? transform_(v) : v);
        }
        return result;
    }

    // Sum values after transformation
    double sum_transformed(const std::vector<double>& values) {
        double total = 0.0;
        for (double v : values) {
            total += transform_ ? transform_(v) : v;
        }
        return total;
    }

private:
    TransformCallback transform_;
};
