#pragma once
#include <string>
#include <string_view>

// Test class with different string types
// Demonstrates binding of std::string and std::string_view
struct Document {
    std::string title = "";
    std::string content = "";

    // Note: string_view fields are tricky because they don't own their data
    // In practice, you'd typically use string_view for function parameters, not member variables
    // But we include it here to test the conversion support
    std::string_view preview = "";  // Will be backed by content or title

    int word_count = 0;
};
