# Mirror Bridge - Quick Start

## First Time Setup (30-60 minutes)

```bash
./start_dev_container.sh
```

This will:
1. Build Docker image with clang-p2996 + <meta> header (~30-60 min)
2. Create persistent container `mirror_bridge_dev`
3. Drop you into bash shell

## Every Day After That (Instant)

```bash
./start_dev_container.sh
```

Instantly attaches to your existing container - all your work is still there!

## Inside Container - Build and Test

```bash
# Build the bindings
cd tests
../build_bindings.sh

# Run tests
python3 test_vector3.py

# Exit (container keeps running)
exit
```

## Common Commands

### From Your Host

```bash
# Start/attach to container
./start_dev_container.sh

# Stop container (preserves state)
docker stop mirror_bridge_dev

# Remove container (start fresh next time)
docker rm -f mirror_bridge_dev

# Check if running
docker ps | grep mirror_bridge
```

### Inside Container

```bash
# Build all bindings
cd /workspace/tests && ../build_bindings.sh

# Build specific binding
clang++ -std=c++2c -freflection -freflection-latest \
  -fPIC -shared \
  $(python3-config --includes --ldflags) \
  vector3_binding.cpp -o ../build/vector3.so

# Run Python tests
python3 test_vector3.py

# Check compiler version
clang++ --version

# Install packages (persists!)
apt-get update && apt-get install -y vim
```

## File Structure

```
/workspace/              # Your project (mounted from host)
├── mirror_bridge.hpp    # Single-header library
├── build/               # Compiled .so files (persisted in container)
├── tests/               # Test cases
│   ├── vector3.hpp      # Example C++ class
│   ├── vector3_binding.cpp
│   └── test_vector3.py
└── build_bindings.sh    # Build script
```

## Workflow

1. **Edit code on your host** (in your favorite editor)
   - Changes appear instantly in `/workspace` inside container

2. **Compile in container**
   ```bash
   ./start_dev_container.sh
   cd tests && ../build_bindings.sh
   ```

3. **Test in container**
   ```bash
   python3 test_vector3.py
   ```

4. **Repeat** - container persists everything!

## Troubleshooting

**Container won't start?**
```bash
docker rm -f mirror_bridge_dev
./start_dev_container.sh
```

**Need clean slate?**
```bash
docker rm -f mirror_bridge_dev
docker rmi mirror_bridge:withmeta
./start_dev_container.sh  # Rebuilds
```

**See more help:**
- [DEVELOPMENT.md](DEVELOPMENT.md) - Complete guide
- [README.md](README.md) - Project overview
- [write_up.md](write_up.md) - Technical details

## What Gets Persisted?

✅ **Persisted between sessions:**
- Compiled bindings in `/workspace/build/`
- Installed apt/pip packages
- Shell history
- Any files you create

✅ **Synced with host (instant):**
- All files in `/workspace` (your project directory)
- Edit on host → instantly visible in container

❌ **Not persisted (container restart):**
- Running processes
- `/tmp` contents

## Next Steps

See [DEVELOPMENT.md](DEVELOPMENT.md) for:
- Multiple terminal sessions
- VS Code integration
- Advanced Docker usage
- Full troubleshooting guide
