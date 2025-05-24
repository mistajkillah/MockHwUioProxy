#!/bin/bash
set -e

SCRIPT_DIR=$(dirname "$(realpath "$0")")
KO_PATH="$SCRIPT_DIR/MockHwUioProxyDriver.ko"

# Load UIO subsystem if not already loaded
echo "[*] Ensuring uio modules are loaded..."
sudo modprobe uio
sudo modprobe uio_pdrv_genirq

if [ ! -f "$KO_PATH" ]; then
    echo "❌ Driver not found at $KO_PATH. Please run 'make' first."
    exit 1
fi

echo "[*] Installing kernel module from $KO_PATH..."
sudo insmod "$KO_PATH"
echo "[✓] Driver installed successfully."
