#!/bin/bash
set -e
SCRIPT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
KO_PATH="$SCRIPT_DIR/output/MockHwUioProxyDriver.ko"

if [ ! -f "$KO_PATH" ]; then
    echo "Driver not found at $KO_PATH. Please run 'make' first."
    exit 1
fi

echo "Installing kernel module..."
sudo insmod "$KO_PATH"

echo "Driver installed successfully."
