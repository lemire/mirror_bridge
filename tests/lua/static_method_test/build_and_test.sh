#!/bin/bash
# Build and test Vec3 Lua bindings
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Building vec3 Lua module..."

# Get Lua include path
LUA_INCLUDE="/usr/include/lua5.4"
LUA_LIB="lua5.4"

clang++ -std=c++2c -freflection -freflection-latest -stdlib=libc++ \
    -shared -fPIC \
    -I"$LUA_INCLUDE" \
    -I"$SCRIPT_DIR/../../.." \
    -o vec3.so \
    vec3_binding.cpp \
    -l$LUA_LIB

echo "Build complete: vec3.so"
echo ""
echo "Running tests..."
lua5.4 test_vec3.lua
