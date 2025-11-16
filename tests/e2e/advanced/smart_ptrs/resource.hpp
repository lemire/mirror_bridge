#pragma once
#include <memory>
#include <string>

// Simple data class for smart pointer elements
struct Data {
    std::string name;
    int value;
};

// Test class for smart pointer support
struct ResourceManager {
    std::unique_ptr<Data> unique_data;
    std::shared_ptr<Data> shared_data;
    double counter = 0.0;

    // Method returning unique_ptr
    std::unique_ptr<Data> create_unique(std::string name, int value) {
        return std::make_unique<Data>(Data{name, value});
    }

    // Method returning shared_ptr
    std::shared_ptr<Data> create_shared(std::string name, int value) {
        return std::make_shared<Data>(Data{name, value});
    }

    // Method taking unique_ptr (will be converted from Python)
    void set_unique(std::unique_ptr<Data> ptr) {
        unique_data = std::move(ptr);
    }

    // Method taking shared_ptr
    void set_shared(std::shared_ptr<Data> ptr) {
        shared_data = ptr;
    }

    // Method to check unique_ptr content
    std::string get_unique_name() const {
        if (unique_data) {
            return unique_data->name;
        }
        return "null";
    }

    // Method to check shared_ptr content
    std::string get_shared_name() const {
        if (shared_data) {
            return shared_data->name;
        }
        return "null";
    }

    int get_unique_value() const {
        return unique_data ? unique_data->value : -1;
    }

    int get_shared_value() const {
        return shared_data ? shared_data->value : -1;
    }
};
