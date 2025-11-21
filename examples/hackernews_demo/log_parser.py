#!/usr/bin/env python3
"""
Log Parser - Python implementation
Searches through log files for patterns, extracts IP addresses, counts errors, etc.
"""

import re
import time
from collections import defaultdict


class LogParser:
    """Parse and analyze log files - pure Python implementation."""

    def __init__(self):
        self.ip_pattern = re.compile(r'\b(?:\d{1,3}\.){3}\d{1,3}\b')
        self.email_pattern = re.compile(r'\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Z|a-z]{2,}\b')

    def count_lines(self, text):
        """Count total lines in text."""
        return text.count('\n') + 1

    def find_errors(self, text):
        """Find all lines containing ERROR."""
        count = 0
        for line in text.split('\n'):
            if 'ERROR' in line or 'Error' in line or 'error' in line:
                count += 1
        return count

    def extract_ip_addresses(self, text):
        """Extract all IP addresses from text."""
        return self.ip_pattern.findall(text)

    def extract_emails(self, text):
        """Extract all email addresses from text."""
        return self.email_pattern.findall(text)

    def count_word_frequency(self, text):
        """Count frequency of each word (case-insensitive)."""
        words = re.findall(r'\b\w+\b', text.lower())
        freq = defaultdict(int)
        for word in words:
            freq[word] += 1
        return dict(freq)

    def find_pattern(self, text, pattern):
        """
        Find all occurrences of a pattern in text (case-insensitive).
        This is the HOT PATH - gets called repeatedly!

        Using a character-by-character approach to show Python's overhead.
        """
        if not pattern or not text:
            return 0

        count = 0
        pattern_lower = pattern.lower()
        pattern_len = len(pattern)
        text_len = len(text)

        # Character-by-character search to show Python's loop overhead
        for i in range(text_len - pattern_len + 1):
            # Check if pattern matches at position i (case-insensitive)
            match = True
            for j in range(pattern_len):
                if text[i + j].lower() != pattern_lower[j]:
                    match = False
                    break

            if match:
                count += 1

        return count

    def search_multiple_patterns(self, text, patterns):
        """Search for multiple patterns - calls find_pattern repeatedly."""
        results = {}
        for pattern in patterns:
            results[pattern] = self.find_pattern(text, pattern)
        return results


def generate_fake_logs(size_mb=10):
    """Generate fake log data for testing."""
    log_template = """
2024-01-15 10:23:45 INFO [server] Request from 192.168.1.{ip} - GET /api/users
2024-01-15 10:23:46 ERROR [database] Connection timeout to db.example.com
2024-01-15 10:23:47 INFO [auth] User john.doe@example.com logged in successfully
2024-01-15 10:23:48 WARNING [cache] Cache miss for key user_session_12345
2024-01-15 10:23:49 ERROR [api] Rate limit exceeded for IP 10.0.{subnet}.{ip}
2024-01-15 10:23:50 INFO [worker] Processing job queue, 42 jobs pending
2024-01-15 10:23:51 ERROR [payment] Transaction failed for user payment_user@gmail.com
2024-01-15 10:23:52 INFO [monitoring] CPU usage: 45%, Memory: 2.1GB, Disk: 78%
2024-01-15 10:23:53 DEBUG [sql] Query executed in 23ms: SELECT * FROM users WHERE active=true
2024-01-15 10:23:54 INFO [websocket] Client 172.16.{subnet}.{ip} connected to room #general
"""

    # Generate logs
    logs = []
    target_size = size_mb * 1024 * 1024

    while len(''.join(logs)) < target_size:
        for ip in range(1, 255):
            for subnet in range(1, 10):
                logs.append(log_template.format(ip=ip, subnet=subnet))

    return ''.join(logs)


if __name__ == '__main__':
    print("=" * 70)
    print("  Python Log Parser Demo")
    print("=" * 70)
    print()

    # Generate test data
    print("Generating 10 MB of log data...")
    logs = generate_fake_logs(10)
    print(f"Generated {len(logs):,} characters ({len(logs)/1024/1024:.1f} MB)")
    print(f"Lines: {logs.count(chr(10)):,}")
    print()

    parser = LogParser()

    # Test 1: Count lines
    print("[1/5] Counting lines...")
    start = time.time()
    line_count = parser.count_lines(logs)
    elapsed = time.time() - start
    print(f"  Result: {line_count:,} lines in {elapsed:.3f}s")
    print()

    # Test 2: Find errors
    print("[2/5] Finding errors...")
    start = time.time()
    error_count = parser.find_errors(logs)
    elapsed = time.time() - start
    print(f"  Result: {error_count:,} errors in {elapsed:.3f}s")
    print()

    # Test 3: Extract IPs
    print("[3/5] Extracting IP addresses...")
    start = time.time()
    ips = parser.extract_ip_addresses(logs)
    elapsed = time.time() - start
    print(f"  Result: {len(ips):,} IPs in {elapsed:.3f}s")
    print()

    # Test 4: Search for specific pattern (THE SLOW PART!)
    print("[4/5] Searching for 'ERROR' pattern (HOT PATH)...")
    start = time.time()
    error_matches = parser.find_pattern(logs, "ERROR")
    elapsed = time.time() - start
    print(f"  Result: {error_matches:,} matches in {elapsed:.3f}s  ⬅ SLOW!")
    print()

    # Test 5: Multiple pattern search
    print("[5/5] Searching for multiple patterns...")
    patterns = ["ERROR", "WARNING", "INFO", "DEBUG", "timeout", "failed", "success"]
    start = time.time()
    results = parser.search_multiple_patterns(logs, patterns)
    elapsed = time.time() - start
    print(f"  Results:")
    for pattern, count in results.items():
        print(f"    {pattern}: {count:,}")
    print(f"  Total time: {elapsed:.3f}s  ⬅ VERY SLOW!")
    print()

    print("=" * 70)
    print("  Pattern searching is the bottleneck!")
    print("  Let's rewrite just find_pattern() in C++...")
    print("=" * 70)
