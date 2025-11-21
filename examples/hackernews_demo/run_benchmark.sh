#!/bin/bash
# Run the benchmark with proper library paths
export LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH
python3 benchmark.py "$@"
