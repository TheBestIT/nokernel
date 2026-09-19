#!/bin/bash
set -euo pipefail

mkdir -p build/esp/EFI/BOOT
cp build/BOOTX64.EFI build/esp/EFI/BOOT/BOOTX64.EFI
cp build/kernel.elf build/esp/kernel.elf