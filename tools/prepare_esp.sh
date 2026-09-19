#!/bin/bash
set -euo pipefail

mkdir -p build/esp/EFI/BOOT
mkdir -p build/esp/assets
cp build/BOOTX64.EFI build/esp/EFI/BOOT/BOOTX64.EFI
cp build/kernel.elf build/esp/kernel.elf
cp assets/fonts/terminus32x16.bmp build/esp/assets/terminus32x16.bmp