#!/bin/bash
# Start or attach to persistent mirror_bridge development container
# This container persists between sessions - no rebuild needed!

set -e

CONTAINER_NAME="mirror_bridge_dev"
IMAGE_NAME="mirror_bridge:withmeta"

# Check if image exists, if not build it
if ! docker image inspect "$IMAGE_NAME" >/dev/null 2>&1; then
    echo "=== Building $IMAGE_NAME (first time only) ==="
    echo "This will take ~1 minute to add the <meta> header..."
    docker build -f Dockerfile.withmeta -t "$IMAGE_NAME" .
    echo "✓ Image built successfully!"
    echo ""
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
