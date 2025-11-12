#!/bin/bash
# Mirror Bridge - Incremental binding compilation script
# Selectively recompiles only classes whose signatures have changed

set -e

# Configuration
CXX="${CXX:-clang++}"
PYTHON_CONFIG="${PYTHON_CONFIG:-python3-config}"
BUILD_DIR="build"
HASH_DB="$BUILD_DIR/class_hashes.db"

# Compiler flags for reflection and Python bindings
CXXFLAGS="-std=c++2c -freflection -freflection-latest -fPIC -shared"
PYTHON_INCLUDES=$($PYTHON_CONFIG --includes)
PYTHON_LDFLAGS=$($PYTHON_CONFIG --ldflags)

echo "=== Mirror Bridge Binding Builder ==="
echo ""

# Verify we have the reflection-enabled compiler
if ! $CXX --version | grep -q "reflection"; then
    echo "Warning: Compiler may not support reflection."
    echo "Current compiler: $CXX"
    echo "Make sure you're using clang with P2996 reflection support."
    echo ""
fi

# Create build directory if it doesn't exist
mkdir -p "$BUILD_DIR"

# Initialize hash database if it doesn't exist
if [ ! -f "$HASH_DB" ]; then
    echo "Initializing hash database..."
    touch "$HASH_DB"
fi

# Function to compute hash of a class definition
compute_class_hash() {
    local source_file=$1
    local class_name=$2

    # Extract class definition and compute SHA256 hash
    # This includes member variables and their types
    grep -A 50 "struct $class_name\|class $class_name" "$source_file" | \
        head -n 50 | \
        sha256sum | \
        awk '{print $1}'
}

# Function to check if class needs recompilation
needs_recompilation() {
    local source_file=$1
    local class_name=$2

    local current_hash=$(compute_class_hash "$source_file" "$class_name")
    local stored_hash=$(grep "^$class_name:" "$HASH_DB" 2>/dev/null | cut -d: -f2)

    if [ -z "$stored_hash" ]; then
        echo "true"  # New class, needs compilation
        return
    fi

    if [ "$current_hash" != "$stored_hash" ]; then
        echo "true"  # Hash changed, needs recompilation
        return
    fi

    # Check if .so file exists
    if [ ! -f "$BUILD_DIR/${class_name}.so" ]; then
        echo "true"  # .so missing, needs compilation
        return
    fi

    echo "false"
}

# Function to update hash database
update_hash() {
    local source_file=$1
    local class_name=$2

    local current_hash=$(compute_class_hash "$source_file" "$class_name")

    # Remove old entry if exists
    grep -v "^$class_name:" "$HASH_DB" > "$HASH_DB.tmp" 2>/dev/null || true
    mv "$HASH_DB.tmp" "$HASH_DB"

    # Add new entry
    echo "$class_name:$current_hash" >> "$HASH_DB"
}

# Function to compile a single binding
compile_binding() {
    local source_file=$1
    local class_name=$2
    local output_name="${3:-$class_name}"

    echo "  Compiling binding for class '$class_name'..."

    $CXX $CXXFLAGS $PYTHON_INCLUDES $PYTHON_LDFLAGS \
        -o "$BUILD_DIR/${output_name}.so" \
        "$source_file"

    if [ $? -eq 0 ]; then
        echo "  ✓ Successfully compiled: $BUILD_DIR/${output_name}.so"
        update_hash "$source_file" "$class_name"
        return 0
    else
        echo "  ✗ Compilation failed for $class_name"
        return 1
    fi
}

# Main compilation logic
compile_all_bindings() {
    local bindings_dir="${1:-.}"
    local compiled=0
    local skipped=0
    local failed=0

    echo "Scanning for binding files in: $bindings_dir"
    echo ""

    # Find all .cpp files that contain MIRROR_BRIDGE_MODULE
    for source_file in "$bindings_dir"/*.cpp; do
        [ -f "$source_file" ] || continue

        # Extract class names from MIRROR_BRIDGE_MODULE or bind_class calls
        local classes=$(grep -oP '(?<=bind_class<)[^>]+' "$source_file" 2>/dev/null || true)

        if [ -z "$classes" ]; then
            continue
        fi

        # Get module/output name from file
        local module_name=$(basename "$source_file" .cpp)

        echo "Processing: $source_file"

        # Check if any class in this file needs recompilation
        local needs_rebuild=false
        for class_name in $classes; do
            if [ "$(needs_recompilation "$source_file" "$class_name")" = "true" ]; then
                needs_rebuild=true
                echo "  → Class '$class_name' needs recompilation"
            fi
        done

        if [ "$needs_rebuild" = "true" ]; then
            if compile_binding "$source_file" "$classes" "$module_name"; then
                ((compiled++))
            else
                ((failed++))
            fi
        else
            echo "  ↷ Skipping (up-to-date)"
            ((skipped++))
        fi

        echo ""
    done

    # Summary
    echo "=== Build Summary ==="
    echo "Compiled: $compiled"
    echo "Skipped:  $skipped"
    echo "Failed:   $failed"
    echo ""

    if [ $failed -gt 0 ]; then
        echo "Build completed with errors."
        return 1
    else
        echo "Build successful!"
        return 0
    fi
}

# Clean build artifacts
if [ "$1" = "clean" ]; then
    echo "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    echo "Done."
    exit 0
fi

# Build all bindings
compile_all_bindings "${1:-.}"
