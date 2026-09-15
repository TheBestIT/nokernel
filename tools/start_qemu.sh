#!/bin/bash

if [ -n "$1" ]; then
    echo "[start_qemu] Debugger is listening for gdb at :1234"
    sudo qemu-system-x86_64 \
        -machine q35,accel=kvm:tcg \
        -m 512M \
        -drive if=pflash,format=raw,unit=0,readonly=on,file=build/ovmf/x64/code.fd \
        -drive if=pflash,format=raw,unit=1,readonly=on,file=build/ovmf/x64/vars.fd \
        -drive format=raw,file=fat:rw:build/esp \
        -serial stdio \
        -nic none \
        -d int,guest_errors -D build/qemu.log \
        -s -S \
        -no-reboot -no-shutdown
else
    echo "[start_qemu] Running without Debugger"
    sudo qemu-system-x86_64 \
        -machine q35,accel=kvm:tcg \
        -m 512M \
        -drive if=pflash,format=raw,unit=0,readonly=on,file=build/ovmf/x64/code.fd \
        -drive if=pflash,format=raw,unit=1,readonly=on,file=build/ovmf/x64/vars.fd \
        -drive format=raw,file=fat:rw:build/esp \
        -serial stdio \
        -nic none \
        -d int,guest_errors -D build/qemu.log \
        -no-reboot -no-shutdown
fi