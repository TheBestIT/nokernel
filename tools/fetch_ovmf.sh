#!/bin/bash
set -euo pipefail

GITHUB_TAG="edk2-stable202605-r1"
mkdir -p build/ovmf
curl -L "https://github.com/rust-osdev/ovmf-prebuilt/releases/download/$GITHUB_TAG/$GITHUB_TAG-bin.tar.xz" | tar -xJ -C build/ovmf --strip-components=1
echo "[fetch_ovmf.sh] Fetched OVMF from github.com/rust-osdev/ovmf-prebuilt/releases/tag/$GITHUB_TAG"