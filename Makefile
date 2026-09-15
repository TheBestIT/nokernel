CLANG = clang

EFI_CLANG_FLAGS = -target x86_64-unknown-windows -ffreestanding -fshort-wchar -mno-red-zone -nostdlib -fuse-ld=lld-link -Wl,-subsystem:efi_application -Wl,-entry:efi_main
EFI_C_SRCS:=$(wildcard boot/src/*.c)

all: ovmf BOOTX64
	$ bash tools/prepare_esp.sh

run: all
	$ bash tools/start_qemu.sh

debug: all
	$ bash tools/start_qemu.sh -d

BOOTX64: build/BOOTX64.EFI
build/BOOTX64.EFI: $(EFI_C_SRCS)
	$(CLANG) $(EFI_CLANG_FLAGS) -o $@ $(EFI_C_SRCS)

ovmf: build/ovmf
build/ovmf: 
	$ bash tools/fetch_ovmf.sh

clean:
	rm -rf build/*