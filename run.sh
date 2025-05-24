#!/bin/bash
set -e

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
OUTPUT_DIR="$ROOT_DIR/output"
MODULE_NAME="MockHwUioProxyDriver"
KO_PATH="$OUTPUT_DIR/${MODULE_NAME}.ko"
INSTALL_SCRIPT="$OUTPUT_DIR/install_driver.sh"
APP_A="$OUTPUT_DIR/ExampleApp"
APP_B="$OUTPUT_DIR/MockHWEmulator"

echo "[*] Attempting to unload old kernel module if loaded..."

# Kill any users of /dev/uio* to safely remove the module
sudo fuser -km /dev/uio* 2>/dev/null || true

# Try unloading the module, retrying if still in use
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
    echo "❌ Could not unload $MODULE_NAME. It may still be in use."
else
    echo "[✓] Module $MODULE_NAME unloaded successfully."
fi

echo "[*] Installing kernel module using $INSTALL_SCRIPT..."
bash "$INSTALL_SCRIPT"
sleep 1

# Launch helper
launch_terminal() {
    local cmd="$1"
    local title="$2"

    if command -v gnome-terminal &>/dev/null; then
        gnome-terminal --title="$title" -- bash -c "$cmd; echo; read -p 'Press Enter to close...'"
    elif command -v konsole &>/dev/null; then
        konsole --new-tab -p tabtitle="$title" -e bash -c "$cmd; echo; read -p 'Press Enter to close...'"
    elif command -v xfce4-terminal &>/dev/null; then
        xfce4-terminal --title="$title" -e "bash -c '$cmd; echo; read -p Press Enter to close...'"
    elif command -v xterm &>/dev/null; then
        xterm -T "$title" -e "$cmd; echo; read -p 'Press Enter to close...'" &
    else
        echo "[!] No supported terminal found. Running $title in background..."
        bash -c "$cmd" &
    fi
}

echo "[*] Launching ExampleApp..."
launch_terminal "$APP_A" "ExampleApp"

echo "[*] Launching MockHWEmulator..."
launch_terminal "$APP_B" "MockHWEmulator"

echo "[✓] All components launched."
