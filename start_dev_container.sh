#!/bin/bash
# Start or attach to persistent mirror_bridge development container
# This container persists between sessions - no rebuild needed!

set -e

CONTAINER_NAME="mirror_bridge_dev"
IMAGE_NAME="mirror_bridge:latest"
REMOTE_IMAGE="ghcr.io/franciscothiesen/mirror_bridge:latest"

# Check if Docker is running
if ! docker info >/dev/null 2>&1; then
    echo "❌ Error: Docker is not running"
    echo ""
    echo "Please start Docker Desktop or the Docker daemon, then try again."
    exit 1
fi

# Check if Dockerfile exists
if [ ! -f "Dockerfile" ]; then
    echo "❌ Error: Dockerfile not found"
    echo ""
    echo "Are you running this from the mirror_bridge root directory?"
    echo "Current directory: $(pwd)"
    exit 1
fi

# Check if image exists, if not try to pull or build it
if ! docker image inspect "$IMAGE_NAME" >/dev/null 2>&1; then
    echo "=== Mirror Bridge Docker Image Not Found ==="
    echo ""
    echo "Choose an option:"
    echo "  1) Pull pre-built image (⚡ fast, ~2 minutes)"
    echo "  2) Build from source (🐌 slow, ~30-60 minutes)"
    echo ""
    read -p "Enter choice [1/2]: " choice
    echo ""

    case $choice in
        1)
            echo "=== Pulling pre-built image ==="
            echo "Downloading from: $REMOTE_IMAGE"
            echo ""

            if docker pull "$REMOTE_IMAGE"; then
                docker tag "$REMOTE_IMAGE" "$IMAGE_NAME"
                echo ""
                echo "✓ Image pulled and tagged successfully!"
                echo ""
            else
                echo ""
                echo "❌ Failed to pull pre-built image"
                echo ""
                echo "The pre-built image may not be available yet."
                echo "Please choose option 2 to build from source."
                echo ""
                exit 1
            fi
            ;;
        2)
            echo "=== Building $IMAGE_NAME from source ==="
            echo "⏱️  This will take ~30-60 minutes to build clang-p2996 with reflection support..."
            echo ""
            echo "What's being built:"
            echo "  • Bloomberg clang-p2996 (C++26 reflection compiler)"
            echo "  • libc++ with reflection support"
            echo "  • Python development headers"
            echo "  • Node.js and Lua support"
            echo ""
            echo "☕ Go grab coffee - this only happens once!"
            echo ""

            if ! docker build -t "$IMAGE_NAME" .; then
                echo ""
                echo "❌ Docker build failed!"
                echo ""
                echo "Common issues:"
                echo "  1. Out of disk space - Docker needs ~10GB for this build"
                echo "  2. Network timeout - Try again, Docker caches progress"
                echo "  3. Memory limits - Docker needs at least 4GB RAM allocated"
                echo ""
                exit 1
            fi

            echo "✓ Image built successfully!"
            echo ""
            ;;
        *)
            echo "Invalid choice. Exiting."
            exit 1
            ;;
    esac
fi

# Check if container exists
if docker ps -a --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
    # Container exists - check if it's running
    if docker ps --format '{{.Names}}' | grep -q "^${CONTAINER_NAME}$"; then
        echo "=== Attaching to running container: $CONTAINER_NAME ==="
        docker exec -it "$CONTAINER_NAME" bash
    else
        echo "=== Starting existing container: $CONTAINER_NAME ==="
        docker start "$CONTAINER_NAME"
        docker exec -it "$CONTAINER_NAME" bash
    fi
else
    # Container doesn't exist - create it
    echo "=== Creating new persistent container: $CONTAINER_NAME ==="
    echo ""
    echo "This container will persist all your changes, including:"
    echo "  - Compiled bindings in /workspace/build/"
    echo "  - Installed packages"
    echo "  - Shell history"
    echo "  - Any files you create"
    echo ""
    echo "The /workspace directory is mounted from $(pwd)"
    echo "Changes to files in /workspace are immediately visible on your host"
    echo ""

    docker run -it \
        --name "$CONTAINER_NAME" \
        -v "$(pwd):/workspace" \
        -w /workspace \
        "$IMAGE_NAME" \
        bash
fi

echo ""
echo "=== Container Usage Tips ==="
echo ""
echo "To attach again:        ./start_dev_container.sh"
echo "To stop container:      docker stop $CONTAINER_NAME"
echo "To remove container:    docker rm $CONTAINER_NAME"
echo "To see container logs:  docker logs $CONTAINER_NAME"
echo ""
echo "Inside container, run:"
echo "  cd tests && ../build_bindings.sh    # Build bindings"
echo "  python3 test_vector3.py             # Run tests"
echo ""
