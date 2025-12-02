#!/bin/bash
# Build and test Vec3 JavaScript bindings
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

echo "Installing dependencies..."
npm install --silent

echo ""
echo "Building vec3 JavaScript module..."
# Set CXX to clang++ with reflection support
export CXX="clang++"
export CC="clang"
npm run build

echo ""
echo "Running tests..."
npm test
