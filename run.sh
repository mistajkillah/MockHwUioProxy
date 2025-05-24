#!/bin/bash
set -e

# Resolve base path
ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
OUTPUT_DIR="$ROOT_DIR/output"
MODULE_NAME="MockHwUioProxyDriver"
KO_PATH="$OUTPUT_DIR/${MODULE_NAME}.ko"
INSTALL_SCRIPT="$OUTPUT_DIR/install_driver.sh"
APP_A="$OUTPUT_DIR/ExampleApp"
APP_B="$OUTPUT_DIR/MockHWEmulator"

# Step 1: Remove old kernel module
echo "[*] Unloading old kernel module if loaded..."
if lsmod | grep -q "$MODULE_NAME"; then
    sudo rmmod "$MODULE_NAME"
    sleep 1
fi

# Step 2: Install the kernel module
if [[ ! -f "$KO_PATH" || ! -f "$INSTALL_SCRIPT" ]]; then
    echo "❌ Kernel module or install script missing in $OUTPUT_DIR. Run 'make' first."
    exit 1
fi

echo "[*] Installing kernel module using $INSTALL_SCRIPT..."
bash "$INSTALL_SCRIPT"
sleep 1

# Function to launch a command in a new terminal window
launch_terminal() {
    local cmd="$1"
    local title="$2"

    if command -v gnome-terminal &>/dev/null; then
        gnome-terminal --title="$title" -- bash -c "$cmd; echo; read -p 'Press Enter to exit...'"
    elif command -v konsole &>/dev/null; then
        konsole --new-tab -p tabtitle="$title" -e bash -c "$cmd; echo; read -p 'Press Enter to exit...'"
    elif command -v xfce4-terminal &>/dev/null; then
        xfce4-terminal --title="$title" -e "bash -c '$cmd; echo; read -p Press Enter to exit...'"
    elif command -v xterm &>/dev/null; then
        xterm -T "$title" -e "$cmd; echo; read -p 'Press Enter to exit...'"
    else
        echo "[!] No supported terminal found. Running $title in background..."
        bash -c "$cmd" &
    fi
}

# Step 3: Launch applications
if [[ -x "$APP_A" && -x "$APP_B" ]]; then
    echo "[*] Launching ExampleApp..."
    launch_terminal "$APP_A" "ExampleApp"

    echo "[*] Launching MockHWEmulator..."
    launch_terminal "$APP_B" "MockHWEmulator"
else
    echo "❌ Compiled applications not found. Please run 'make' first."
    exit 1
fi

echo "[✓] Everything launched."
