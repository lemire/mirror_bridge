#!/usr/bin/env python3
"""
Pure Python implementation of TextAnalyzer for comparison.
"""


class TextAnalyzer:
    def __init__(self, text=""):
        self.text = text

    def set_text(self, text):
        self.text = text

    def get_text(self):
        return self.text

    def count_chars(self):
        return len(self.text)

    def count_words(self):
        if not self.text:
            return 0
        return len(self.text.split())

    def count_lines(self):
        if not self.text:
            return 0
        return self.text.count('\n') + 1

    def count_vowels(self):
        vowels = 'aeiouAEIOU'
        return sum(1 for c in self.text if c in vowels)

    def count_consonants(self):
        consonants = 'bcdfghjklmnpqrstvwxyzBCDFGHJKLMNPQRSTVWXYZ'
        return sum(1 for c in self.text if c in consonants)

    def most_common_char(self):
        if not self.text:
            return ''

        freq = {}
        for c in self.text:
            if not c.isspace():
                c_lower = c.lower()
                freq[c_lower] = freq.get(c_lower, 0) + 1

        if not freq:
            return ''

        return max(freq.items(), key=lambda x: x[1])[0]

    def reverse(self):
        return self.text[::-1]

    def to_uppercase(self):
        return self.text.upper()

    def to_lowercase(self):
        return self.text.lower()

    def is_palindrome(self):
        clean = ''.join(c.lower() for c in self.text if not c.isspace())
        return clean == clean[::-1]


if __name__ == '__main__':
    # Quick test
    analyzer = TextAnalyzer("Hello World! This is a test.")
    print(f"Characters: {analyzer.count_chars()}")
    print(f"Words: {analyzer.count_words()}")
    print(f"Vowels: {analyzer.count_vowels()}")
    print(f"Most common: '{analyzer.most_common_char()}'")
    print(f"Uppercase: {analyzer.to_uppercase()}")
