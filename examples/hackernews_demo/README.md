# HackerNews Demo: Surgical Python Optimization

**The problem every developer faces**: Your Python code works great, but one function is killing performance.

**The traditional solution**: Rewrite in C++, then spend hours writing pybind11 boilerplate.

**The Mirror Bridge way**: Replace just the hot function with C++. Zero binding code. **7.7x faster.**

## What This Demo Shows

This is a realistic log parser that searches for patterns in log files. The bottleneck? A single function: `find_pattern()`.

**Key insight**: You don't need to rewrite everything in C++. Just replace the hot path.

## The Setup

```python
# Python implementation - works but slow
class LogParser:
    def find_pattern(self, text, pattern):
        # Character-by-character search
        # Python's loop overhead kills performance
        for i in range(len(text) - len(pattern) + 1):
            match = True
            for j in range(len(pattern)):
                if text[i + j].lower() != pattern[j].lower():
                    match = False
                    break
            if match:
                count += 1
        return count
```

Searching through 50 MB of logs for 7 patterns: **45 seconds** 🐌

## The Solution

Write just the hot function in C++:

```cpp
class PatternMatcher {
public:
    int find_pattern(const std::string& text,
                    const std::string& pattern) const {
        // Same algorithm, but compiled
        // No Python interpreter overhead
        ...
    }
};
```

Generate bindings automatically:

```bash
mirror_bridge_auto . --module pattern_matcher
```

**Zero lines of binding code written.**

Use it from Python:

```python
import pattern_matcher

# Drop-in replacement for the slow function
matcher = pattern_matcher.PatternMatcher()
count = matcher.find_pattern(logs, "ERROR")  # Fast!
```

Same 50 MB search: **5.9 seconds** 🚀

## Results

```
Operation                    Python      C++     Speedup
──────────────────────────────────────────────────────────
Single Pattern              5.7s       0.8s      6.8x
Multiple Patterns          39.3s       5.0s      7.8x
──────────────────────────────────────────────────────────
TOTAL                      45.0s       5.9s      7.7x
```

**45 seconds → 5.9 seconds. 87% faster.**

## Why This Matters

1. **Surgical optimization**: You changed ONE function, not your entire codebase
2. **Zero boilerplate**: No pybind11, no SWIG, no manual bindings
3. **Same algorithm**: Just replaced Python's slow loops with compiled code
4. **Easy maintenance**: When your API changes, regenerate bindings in 1 command

## Quick Start

```bash
# Inside Docker
docker exec -it mirror_bridge_dev bash
cd /workspace/examples/hackernews_demo

# Step 1: See the problem (Python is slow)
python3 log_parser.py

# Step 2: Generate C++ bindings
./run_auto_discovery.sh

# Step 3: See the solution (C++ is fast)
./run_benchmark.sh
```

## The Takeaway for HackerNews

**Before Mirror Bridge:**
- Profile Python code
- Find the hot function
- Rewrite in C++
- Spend 2 hours writing pybind11 boilerplate
- Manually update bindings every time your API changes

**With Mirror Bridge:**
- Profile Python code
- Find the hot function
- Rewrite in C++
- Run: `mirror_bridge_auto . --module yourmodule`

**Done. Zero binding code.**

This is C++26 reflection in action. The compiler introspects your C++ class and generates perfect Python bindings automatically.

## Real-World Use Cases

This pattern works for any hot function:

- 🔍 **Text parsing** - regex, search, tokenization
- 📊 **Data processing** - sorting, filtering, aggregation
- 🔢 **Math-heavy code** - numerical methods, simulations
- 🎮 **Game logic** - pathfinding, physics, collision detection
- 🔐 **Cryptography** - hashing, encryption (when you can't use C libraries)

The sweet spot: **Python for glue code, C++ for the hot 5% of your program**.

## What Makes This Different

**pybind11 approach** (traditional):
```cpp
PYBIND11_MODULE(pattern_matcher, m) {
    py::class_<PatternMatcher>(m, "PatternMatcher")
        .def(py::init<>())
        .def("find_pattern", &PatternMatcher::find_pattern)
        .def("search_multiple_patterns",
             &PatternMatcher::search_multiple_patterns);
}
```
~30 lines for this simple class. Grows with every method.

**Mirror Bridge approach**:
```bash
mirror_bridge_auto . --module pattern_matcher
```
0 lines. Auto-updates when your API changes.

## Files

- `log_parser.py` - Pure Python implementation (slow)
- `pattern_matcher.hpp` - C++ hot function (fast)
- `benchmark.py` - Head-to-head comparison
- `run_auto_discovery.sh` - Generate bindings
- `run_benchmark.sh` - See the speedup

Try it yourself! This is a real, working example you can run in 3 commands.
