#!/bin/bash
set -e

# --- Configuration ---
SESSION_NAME="MockHwUIO"
ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
OUTPUT_DIR="$ROOT_DIR/output"
INSTALL_SCRIPT="$OUTPUT_DIR/install_driver.sh"
KO_PATH="$OUTPUT_DIR/MockHwUioProxyDriver.ko"
APP_A="$OUTPUT_DIR/ExampleApp"
APP_B="$OUTPUT_DIR/MockHWEmulator"
MODULE_NAME="MockHwUioProxyDriver"

# --- Load Kernel Module Dependencies ---
echo "[*] Loading required kernel modules..."
sudo modprobe uio
sudo modprobe uio_pdrv_genirq

# --- Unload Old Module Safely ---
echo "[*] Unloading old kernel module if loaded..."
sudo fuser -km /dev/uio* 2>/dev/null || true

for i in {1..10}; do
    if ! lsmod | grep -q "$MODULE_NAME"; then
        break
    fi
    if sudo rmmod "$MODULE_NAME"; then
        echo "[*] Module $MODULE_NAME removed."
        break
    fi
    echo "[!] Module still in use. Retrying ($i)..."
    sleep 0.5
done

if lsmod | grep -q "$MODULE_NAME"; then
    echo "❌ Could not remove $MODULE_NAME. It may still be in use."
    exit 1
fi

# --- Install Kernel Driver ---
echo "[*] Installing driver using $INSTALL_SCRIPT..."
if [ ! -f "$INSTALL_SCRIPT" ]; then
    echo "❌ install_driver.sh not found at: $INSTALL_SCRIPT"
    exit 1
fi

bash "$INSTALL_SCRIPT"
echo "[✓] Driver installed successfully."
sleep 1

# --- Launch Apps in tmux ---
echo "[*] Launching apps in tmux session: $SESSION_NAME"

# Kill session if it already exists
tmux has-session -t "$SESSION_NAME" 2>/dev/null && tmux kill-session -t "$SESSION_NAME"

# Launch ExampleApp with hold
tmux new-session -d -s "$SESSION_NAME" -n "ExampleApp" \
    "bash -c '$APP_A; echo; echo \"[ExampleApp exited] Press Enter to close...\"; read'"

# Launch MockHWEmulator with hold
tmux new-window -t "$SESSION_NAME:" -n "MockHWEmulator" \
    "bash -c '$APP_B; echo; echo \"[MockHWEmulator exited] Press Enter to close...\"; read'"

echo
echo "[✓] tmux session '$SESSION_NAME' created with:"
echo "    - Window 0: ExampleApp"
echo "    - Window 1: MockHWEmulator"
echo
echo "▶ Attach with:   tmux attach-session -t $SESSION_NAME"
echo "▶ Switch:        Ctrl+b then n or p"
echo "▶ Detach:        Ctrl+b then d"
