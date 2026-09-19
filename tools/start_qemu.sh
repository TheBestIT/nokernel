#!/bin/bash
# Usage: start_qemu.sh <esp|img> [-d]
#   esp  Boot from the build/esp folder (QEMU virtual FAT drive).
#   img  Boot from the build/loader.img GPT disk image.
#   -d   Stop the CPU at start and listen for gdb on :1234.
set -euo pipefail

usage() {
    echo "Usage: $0 <esp|img> [-d]" >&2
    exit 1
}

[ $# -ge 1 ] || usage
MODE=$1
DEBUG=${2:-}

case "$MODE" in
    esp)
        [ -d build/esp ] || { echo "[start_qemu] build/esp not found. Run 'make esp'." >&2; exit 1; }
        DRIVE="format=raw,file=fat:rw:build/esp"
        ;;
    img)
        [ -f build/loader.img ] || { echo "[start_qemu] build/loader.img not found. Run 'make image'." >&2; exit 1; }
        DRIVE="format=raw,file=build/loader.img"
        ;;
    *)
        usage
        ;;
esac

ARGS=(
    -machine q35,accel=kvm:tcg
    -m 512M
    -drive if=pflash,format=raw,unit=0,readonly=on,file=build/ovmf/x64/code.fd
    -drive if=pflash,format=raw,unit=1,readonly=on,file=build/ovmf/x64/vars.fd
    -drive "$DRIVE"
    -serial stdio
    -nic none
    -d int,guest_errors -D build/qemu.log
    -no-reboot -no-shutdown
)

case "$DEBUG" in
    "")
        echo "[start_qemu] Boot from $MODE without debugger"
        ;;
    -d)
        echo "[start_qemu] Boot from $MODE. Debugger is listening for gdb at :1234"
        ARGS+=(-s -S)
        ;;
    *)
        usage
        ;;
esac

sudo qemu-system-x86_64 "${ARGS[@]}"
