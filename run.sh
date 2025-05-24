#!/bin/bash
set -e

OUTPUT_DIR=./output
DRIVER_SCRIPT="$OUTPUT_DIR/install_driver.sh"
EXAMPLE_APP="$OUTPUT_DIR/ExampleApp"
MOCKHW_APP="$OUTPUT_DIR/MockHWEmulator"

echo "[*] Building project..."
make

echo "[*] Installing driver using $DRIVER_SCRIPT..."
if [ ! -f "$DRIVER_SCRIPT" ]; then
    echo "❌ Driver install script not found: $DRIVER_SCRIPT"
    exit 1
fi

sudo "$DRIVER_SCRIPT"
echo "[✓] Driver installed."

echo "[*] Launching ExampleApp and MockHWEmulator..."

stdbuf -oL -eL "$EXAMPLE_APP" | sed 's/^/[ExampleApp] /' &
PID1=$!

stdbuf -oL -eL "$MOCKHW_APP" | sed 's/^/[MockHWEmulator] /' &
PID2=$!

echo "[✓] Both apps started. PIDs: $PID1, $PID2"
echo "▶ Press Ctrl+C to stop"

# Clean exit
trap "echo '[*] Terminating...'; kill $PID1 $PID2; wait; exit" INT

wait $PID1
wait $PID2
