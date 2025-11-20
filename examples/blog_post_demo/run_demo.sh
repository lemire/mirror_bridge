#!/bin/bash
# Run the C++ demo with proper library paths
export LD_LIBRARY_PATH=/usr/local/lib/aarch64-unknown-linux-gnu:$LD_LIBRARY_PATH
python3 demo.py "$@"
