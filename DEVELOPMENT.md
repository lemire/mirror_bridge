# Development Guide - Persistent Container Setup

This guide shows how to work with a **persistent development container** that keeps all your changes between sessions.

## Quick Start

### First Time Setup

```bash
# 1. Build the base image (if you haven't already)
./docker_build.sh

# 2. Start the persistent dev container
./start_dev_container.sh
```

That's it! The script will:
- Build the `mirror_bridge:withmeta` image (adds `<meta>` header) - **~1 minute**
- Create a persistent container named `mirror_bridge_dev`
- Mount your current directory to `/workspace`
- Drop you into a bash shell

### Daily Workflow

Every time you want to work on the project:

```bash
./start_dev_container.sh
```

This will:
- **Attach to the existing container** if it's running
- **Start and attach** if it's stopped
- **Create new** if it doesn't exist

## What Gets Persisted?

### ✅ Persisted (Inside Container)
- Compiled bindings in `/workspace/build/`
- Installed packages (`apt install`, `pip install`)
- Shell history (`~/.bash_history`)
- Any configuration files you create
- Temporary files in `/tmp` (until container restart)

### ✅ Persisted (On Host via Volume Mount)
Everything in `/workspace` is your actual project directory:
- Source code changes
- New files/folders
- Build artifacts in `build/`
- Test results

### ❌ Not Persisted (Container Restart)
- Running processes
- `/tmp` contents (on container restart)
- Environment variables (unless in `~/.bashrc`)

## Common Operations

### Working Inside Container

```bash
# Start container
./start_dev_container.sh

# Inside container:
cd /workspace/tests

# Build bindings
../build_bindings.sh

# Run tests
python3 test_vector3.py

# Install additional packages (persists!)
apt-get update && apt-get install -y vim

# Exit (container keeps running)
exit
```

### Attaching Again

```bash
# From your host
./start_dev_container.sh

# You're back in the same container!
# All your previous work is still there
```

### Container Management

```bash
# See if container is running
docker ps | grep mirror_bridge_dev

# See all containers (including stopped)
docker ps -a | grep mirror_bridge_dev

# Stop container (but keep it)
docker stop mirror_bridge_dev

# Start stopped container
docker start mirror_bridge_dev

# Remove container completely (start fresh next time)
docker rm -f mirror_bridge_dev

# Remove image (to rebuild from scratch)
docker rmi mirror_bridge:withmeta
```

## Example Session

```bash
# Day 1: First time
$ ./start_dev_container.sh
=== Building mirror_bridge:withmeta (first time only) ===
✓ Image built successfully!
=== Creating new persistent container: mirror_bridge_dev ===

root@abc123:/workspace# cd tests
root@abc123:/workspace/tests# ../build_bindings.sh
# ... builds happen ...
root@abc123:/workspace/tests# exit

# Day 2: Continue work
$ ./start_dev_container.sh
=== Attaching to running container: mirror_bridge_dev ===

root@abc123:/workspace# ls build/
# All your previous builds are still here!
root@abc123:/workspace# python3 tests/test_vector3.py
# Run tests immediately - no rebuild needed
```

## Advanced Usage

### Multiple Terminal Sessions

```bash
# Terminal 1
./start_dev_container.sh
# Work in /workspace/tests

# Terminal 2 (while Terminal 1 is still open)
docker exec -it mirror_bridge_dev bash
# Now you have 2 shells in the same container
```

### Run Commands Without Interactive Shell

```bash
# Build bindings without entering container
docker exec mirror_bridge_dev bash -c "cd /workspace/tests && ../build_bindings.sh"

# Run tests
docker exec mirror_bridge_dev python3 /workspace/tests/test_vector3.py

# Check clang version
docker exec mirror_bridge_dev clang++ --version
```

### Copy Files In/Out of Container

```bash
# Copy file from host to container
docker cp myfile.cpp mirror_bridge_dev:/workspace/tests/

# Copy file from container to host
docker cp mirror_bridge_dev:/workspace/build/output.so ./

# Note: /workspace is already mounted, so just edit files normally on your host!
```

### Clean Start (Keep Image, Remove Container)

```bash
# Remove container but keep image
docker rm -f mirror_bridge_dev

# Next time you run start_dev_container.sh, it creates a fresh container
./start_dev_container.sh
```

### Complete Clean (Rebuild Everything)

```bash
# Remove container
docker rm -f mirror_bridge_dev

# Remove image
docker rmi mirror_bridge:withmeta mirror_bridge:latest

# Rebuild from scratch
./docker_build.sh                 # Base image (~30-60 min)
./start_dev_container.sh         # Adds meta header (~1 min)
```

## Troubleshooting

### Container Won't Start

```bash
# Check if container exists but is corrupted
docker ps -a | grep mirror_bridge_dev

# Remove and recreate
docker rm -f mirror_bridge_dev
./start_dev_container.sh
```

### Out of Disk Space

```bash
# Check Docker disk usage
docker system df

# Clean up stopped containers
docker container prune

# Clean up unused images
docker image prune

# Nuclear option: clean everything
docker system prune -a
```

### Need to Update <meta> Header

```bash
# Stop and remove container
docker rm -f mirror_bridge_dev

# Remove the withmeta image
docker rmi mirror_bridge:withmeta

# Rebuild (will download latest meta header)
docker build -f Dockerfile.withmeta -t mirror_bridge:withmeta .

# Start fresh
./start_dev_container.sh
```

### Reset Build Directory

```bash
# Inside container or on host:
rm -rf /workspace/build
mkdir /workspace/build

# Rebuild bindings
cd /workspace/tests && ../build_bindings.sh
```

## IDE Integration

### VS Code

You can attach VS Code to the running container:

1. Install "Dev Containers" extension
2. Start container: `./start_dev_container.sh`
3. In VS Code: Cmd+Shift+P → "Dev Containers: Attach to Running Container"
4. Select `mirror_bridge_dev`
5. Open `/workspace`

Now VS Code runs inside the container with full access to the compiler!

### CLion / Other IDEs

Configure remote development:
- Host: localhost
- Container: mirror_bridge_dev
- Path: /workspace

Or just use the IDE normally - since `/workspace` is mounted, files sync automatically.

## Comparison: Persistent vs. Disposable Containers

### Persistent (Recommended for Development)
```bash
./start_dev_container.sh          # Creates named container
# Work, exit, work again - everything persists
```

**Pros:**
- Fast subsequent starts
- Packages stay installed
- Build cache preserved
- Shell history works

**Cons:**
- Takes up disk space even when not running
- Can accumulate cruft over time

### Disposable (Good for CI/Testing)
```bash
docker run --rm -v $(pwd):/workspace -it mirror_bridge:withmeta bash
# Everything deleted on exit
```

**Pros:**
- Always clean state
- No disk space used when stopped

**Cons:**
- Slower restarts
- Must reinstall packages each time
- Loses all state

## Best Practices

1. **Use the persistent container for development**
   - Faster iteration
   - Preserves your environment

2. **Edit files on your host**
   - Use your favorite editor/IDE
   - Changes appear instantly in container

3. **Compile inside the container**
   - Ensures correct compiler version
   - Consistent build environment

4. **Periodically clean up**
   ```bash
   # Every few weeks, start fresh
   docker rm -f mirror_bridge_dev
   ./start_dev_container.sh
   ```

5. **Commit working states**
   ```bash
   # When you have working code, commit it
   git add .
   git commit -m "Working Python bindings"
   ```

## Next Steps

Once your container is running:

1. **Build the bindings:**
   ```bash
   cd /workspace/tests
   ../build_bindings.sh
   ```

2. **Run the tests:**
   ```bash
   python3 test_vector3.py
   ```

3. **Iterate quickly:**
   - Edit `.hpp` or `.cpp` files on your host
   - Run `../build_bindings.sh` in container
   - Test immediately

Happy coding! 🚀
