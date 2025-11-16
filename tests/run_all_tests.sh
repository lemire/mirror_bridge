#!/bin/bash
# Mirror Bridge - Automated Test Runner
# Builds all bindings and runs all tests in sequence

set -e  # Exit on first error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BUILD_DIR="$PROJECT_ROOT/build"
TEST_DIR="$PROJECT_ROOT/tests/e2e"
BINDING_SCRIPT="$PROJECT_ROOT/build_bindings.sh"

# Set up environment for libc++ (needed for reflection support)
export LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH

# Counters
TOTAL_BINDINGS=0
BUILT_BINDINGS=0
FAILED_BINDINGS=0
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}  Mirror Bridge - Automated Test Suite${NC}"
echo -e "${BLUE}=======================================${NC}"
echo ""

# Check if running inside Docker container with reflection compiler
if ! command -v clang++ &> /dev/null; then
    echo -e "${RED}Error: clang++ not found${NC}"
    echo "This script must run inside the mirror_bridge Docker container."
    echo ""
    echo "To run tests:"
    echo "  ./start_dev_container.sh"
    echo "  cd /workspace"
    echo "  ./tests/run_all_tests.sh"
    exit 1
fi

# Check if compiler supports reflection
if ! clang++ --version 2>&1 | grep -q "reflection\|p2996\|bloomberg"; then
    echo -e "${YELLOW}Warning: Compiler may not support C++26 reflection${NC}"
    echo "This script requires the reflection-enabled clang compiler."
    echo ""
    echo "To run tests inside the Docker container:"
    echo "  ./start_dev_container.sh"
    echo "  cd /workspace"
    echo "  ./tests/run_all_tests.sh"
    echo ""
    read -p "Continue anyway? [y/N] " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        exit 1
    fi
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Step 1: Build all bindings
echo -e "${YELLOW}[STEP 1/2] Building all bindings...${NC}"
echo ""

cd "$TEST_DIR"

# Find all binding .cpp files recursively
while IFS= read -r -d '' binding_file; do
    [ -f "$binding_file" ] || continue

    TOTAL_BINDINGS=$((TOTAL_BINDINGS + 1))

    # Extract module name from file (remove _binding.cpp)
    module_name=$(basename "$binding_file" _binding.cpp)

    # Get the directory of the binding file for proper includes
    binding_dir=$(dirname "$binding_file")

    echo -e "${BLUE}Building ${module_name}...${NC}"

    # Compile the binding - capture output
    # Add both project root and binding dir to include paths
    compile_output=$(cd "$binding_dir" && clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
        -I"$PROJECT_ROOT" -I. -fPIC -shared \
        $(python3-config --includes --ldflags) \
        "$(basename "$binding_file")" -o "$BUILD_DIR/${module_name}.so" 2>&1)

    compile_exit=$?

    # Filter out the designated initializer warning
    filtered_output=$(echo "$compile_output" | grep -v "mixture of designated" || true)

    # Check if compilation succeeded
    if [ $compile_exit -eq 0 ]; then
        echo -e "${GREEN}✓ Built: ${module_name}.so${NC}"
        BUILT_BINDINGS=$((BUILT_BINDINGS + 1))
    else
        echo -e "${RED}✗ Failed: ${module_name}${NC}"
        FAILED_BINDINGS=$((FAILED_BINDINGS + 1))
        # Show compilation errors
        if [ -n "$filtered_output" ]; then
            echo "$filtered_output" | sed 's/^/  /'
        fi
    fi
    echo ""
done < <(find . -name "*.cpp" -type f -print0)

# Step 2: Run all Python tests
echo -e "${YELLOW}[STEP 2/2] Running Python tests...${NC}"
echo ""

# Set PYTHONPATH to include build directory
export PYTHONPATH="$BUILD_DIR:$PYTHONPATH"

# Tests to skip (with reasons)
SKIP_TESTS=()

# Function to check if test should be skipped
should_skip_test() {
    local test_name=$1
    for skip_entry in "${SKIP_TESTS[@]}"; do
        local skip_test=$(echo "$skip_entry" | cut -d: -f1)
        if [ "$test_name" = "$skip_test" ]; then
            return 0  # Should skip
        fi
    done
    return 1  # Should not skip
}

# Function to get skip reason
get_skip_reason() {
    local test_name=$1
    for skip_entry in "${SKIP_TESTS[@]}"; do
        local skip_test=$(echo "$skip_entry" | cut -d: -f1)
        local skip_reason=$(echo "$skip_entry" | cut -d: -f2-)
        if [ "$test_name" = "$skip_test" ]; then
            echo "$skip_reason"
            return
        fi
    done
}

# Find all test_*.py files recursively in the test directory
while IFS= read -r -d '' test_file; do
    [ -f "$test_file" ] || continue

    TOTAL_TESTS=$((TOTAL_TESTS + 1))

    test_name=$(basename "$test_file")

    # Check if test should be skipped
    if should_skip_test "$test_name"; then
        echo -e "${YELLOW}Skipping ${test_name}...${NC}"
        echo -e "${YELLOW}  Reason: $(get_skip_reason "$test_name")${NC}"
        echo ""
        continue
    fi

    echo -e "${BLUE}Running ${test_name}...${NC}"

    # Run the test from the test directory so relative imports work
    if (cd "$TEST_DIR" && python3 "$test_file") > /tmp/test_output.txt 2>&1; then
        echo -e "${GREEN}✓ Passed: ${test_name}${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))

        # Show test output if verbose
        if [ -n "$VERBOSE" ]; then
            cat /tmp/test_output.txt | sed 's/^/  /'
        fi
    else
        echo -e "${RED}✗ Failed: ${test_name}${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))

        # Always show failed test output
        echo -e "${RED}Error output:${NC}"
        cat /tmp/test_output.txt | sed 's/^/  /'
    fi
    echo ""
done < <(find "$TEST_DIR" -name "test_*.py" -type f -print0)

# Summary
echo -e "${BLUE}=======================================${NC}"
echo -e "${BLUE}  Test Summary${NC}"
echo -e "${BLUE}=======================================${NC}"
echo ""

echo -e "${YELLOW}Bindings:${NC}"
echo -e "  Total:   $TOTAL_BINDINGS"
echo -e "  ${GREEN}Built:   $BUILT_BINDINGS${NC}"
if [ $FAILED_BINDINGS -gt 0 ]; then
    echo -e "  ${RED}Failed:  $FAILED_BINDINGS${NC}"
fi
echo ""

echo -e "${YELLOW}Tests:${NC}"
echo -e "  Total:   $TOTAL_TESTS"
echo -e "  ${GREEN}Passed:  $PASSED_TESTS${NC}"
if [ $FAILED_TESTS -gt 0 ]; then
    echo -e "  ${RED}Failed:  $FAILED_TESTS${NC}"
fi
echo ""

# Exit status
if [ $FAILED_BINDINGS -eq 0 ] && [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}✓ ALL TESTS PASSED!${NC}"
    exit 0
else
    echo -e "${RED}✗ SOME TESTS FAILED${NC}"
    exit 1
fi
