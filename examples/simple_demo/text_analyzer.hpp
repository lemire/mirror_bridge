// Simple C++ Text Analyzer
// Fast text processing for counting words, characters, and computing statistics

#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>
#include <cctype>

class TextAnalyzer {
private:
    std::string text_;

public:
    // Default constructor
    TextAnalyzer() {}

    // Constructor with text
    TextAnalyzer(const std::string& text) : text_(text) {}

    // Set text to analyze
    void set_text(const std::string& text) {
        text_ = text;
    }

    // Get the text
    std::string get_text() const {
        return text_;
    }

    // Count total characters
    int count_chars() const {
        return text_.length();
    }

    // Count words (space-separated)
    int count_words() const {
        if (text_.empty()) return 0;

        int count = 0;
        bool in_word = false;

        for (char c : text_) {
            if (std::isspace(c)) {
                in_word = false;
            } else if (!in_word) {
                in_word = true;
                count++;
            }
        }

        return count;
    }

    // Count lines
    int count_lines() const {
        if (text_.empty()) return 0;

        int count = 1;
        for (char c : text_) {
            if (c == '\n') count++;
        }
        return count;
    }

    // Count vowels
    int count_vowels() const {
        int count = 0;
        for (char c : text_) {
            char lower = std::tolower(c);
            if (lower == 'a' || lower == 'e' || lower == 'i' ||
                lower == 'o' || lower == 'u') {
                count++;
            }
        }
        return count;
    }

    // Count consonants
    int count_consonants() const {
        int count = 0;
        for (char c : text_) {
            if (std::isalpha(c)) {
                char lower = std::tolower(c);
                if (lower != 'a' && lower != 'e' && lower != 'i' &&
                    lower != 'o' && lower != 'u') {
                    count++;
                }
            }
        }
        return count;
    }

    // Find most common character
    char most_common_char() const {
        if (text_.empty()) return '\0';

        std::unordered_map<char, int> freq;
        for (char c : text_) {
            if (!std::isspace(c)) {
                freq[std::tolower(c)]++;
            }
        }

        char most_common = '\0';
        int max_count = 0;

        for (const auto& pair : freq) {
            if (pair.second > max_count) {
                max_count = pair.second;
                most_common = pair.first;
            }
        }

        return most_common;
    }

    // Reverse the text
    std::string reverse() const {
        std::string reversed = text_;
        std::reverse(reversed.begin(), reversed.end());
        return reversed;
    }

    // Convert to uppercase
    std::string to_uppercase() const {
        std::string upper = text_;
        std::transform(upper.begin(), upper.end(), upper.begin(),
                      [](unsigned char c) { return std::toupper(c); });
        return upper;
    }

    // Convert to lowercase
    std::string to_lowercase() const {
        std::string lower = text_;
        std::transform(lower.begin(), lower.end(), lower.begin(),
                      [](unsigned char c) { return std::tolower(c); });
        return lower;
    }

    // Check if text is palindrome (ignoring spaces and case)
    bool is_palindrome() const {
        std::string clean;
        for (char c : text_) {
            if (!std::isspace(c)) {
                clean += std::tolower(c);
            }
        }

        std::string reversed = clean;
        std::reverse(reversed.begin(), reversed.end());

        return clean == reversed;
    }
};
