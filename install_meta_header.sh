#!/bin/bash
# Quick workaround to add <meta> header to existing container
# This avoids rebuilding the entire container (saves ~1 hour)

set -e

echo "=== Installing <meta> header to mirror_bridge container ==="

# Download meta header from clang-p2996
echo "Downloading meta header from Bloomberg clang-p2996..."
curl -s https://raw.githubusercontent.com/bloomberg/clang-p2996/p2996/libcxx/include/meta -o /tmp/meta_header

# Create directory and copy header into running container
echo "Installing header into container..."
docker run --rm -v /tmp/meta_header:/tmp/meta_source mirror_bridge:latest bash -c "
    mkdir -p /usr/local/include/c++/v1 && \
    cp /tmp/meta_source /usr/local/include/c++/v1/meta && \
    chmod 644 /usr/local/include/c++/v1/meta && \
    ls -la /usr/local/include/c++/v1/meta
"

echo ""
echo "✓ <meta> header installed successfully!"
echo ""
echo "You can now use: #include <meta>"
echo ""
echo "Note: This is a workaround. For a permanent solution, rebuild with Dockerfile.full"

# Clean up
rm -f /tmp/meta_header
