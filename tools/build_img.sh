#!/bin/bash
set -euo pipefail

IMG=build/loader.img
OFF=$((2048*512))

dd if=/dev/zero of=$IMG bs=1M count=64 status=none
sgdisk -Z "$IMG" >/dev/null
sgdisk -n 1:2048:0 -t 1:ef00 -c 1:"EFI System" "$IMG" >/dev/null

mformat -i "$IMG@@$OFF" -F -v ESP ::
mmd     -i "$IMG@@$OFF" ::/EFI ::/EFI/BOOT
mcopy   -i "$IMG@@$OFF" build/BOOTX64.EFI ::/EFI/BOOT/BOOTX64.EFI
mcopy   -i "$IMG@@$OFF" build/kernel.elf ::/kernel.elf