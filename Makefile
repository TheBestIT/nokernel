CLANG := clang
LLD   := $(shell command -v lld-link 2>/dev/null)

BUILD := build
ESP   := $(BUILD)/esp

EFI_INCLUDES := -I boot/src/include -I boot/src

EFI_CFLAGS := -target x86_64-unknown-windows \
              -ffreestanding -fshort-wchar -mno-red-zone -nostdlib \
              -Wall -Wextra -MMD -MP \
              $(EFI_INCLUDES)

EFI_LDFLAGS := -fuse-ld=lld $(if $(LLD),--ld-path=$(LLD)) \
               -Wl,-subsystem:efi_application -Wl,-entry:efi_main

EFI_SRCS := $(shell find boot/src -name '*.c')
EFI_OBJS := $(patsubst boot/src/%.c,$(BUILD)/boot/%.o,$(EFI_SRCS))
EFI_DEPS := $(EFI_OBJS:.o=.d)

OVMF_CODE := $(BUILD)/ovmf/x64/code.fd

.PHONY: all run debug clean ovmf bootx64

all: $(ESP)/EFI/BOOT/BOOTX64.EFI $(OVMF_CODE)

bootx64: $(BUILD)/BOOTX64.EFI
ovmf:    $(OVMF_CODE)

run: all
	bash tools/start_qemu.sh

debug: all
	bash tools/start_qemu.sh -d

$(BUILD)/BOOTX64.EFI: $(EFI_OBJS)
	@test -n "$(LLD)" || { echo "lld-link non trovato: sudo apt install lld"; exit 1; }
	$(CLANG) $(EFI_CFLAGS) $(EFI_LDFLAGS) -o $@ $^

$(BUILD)/boot/%.o: boot/src/%.c
	@mkdir -p $(dir $@)
	$(CLANG) $(EFI_CFLAGS) -c -o $@ $<

$(ESP)/EFI/BOOT/BOOTX64.EFI: $(BUILD)/BOOTX64.EFI
	bash tools/prepare_esp.sh

$(OVMF_CODE):
	bash tools/fetch_ovmf.sh

clean:
	rm -rf $(BUILD)/BOOTX64.EFI $(BUILD)/boot $(ESP)

distclean: clean
	rm -rf $(BUILD)

-include $(EFI_DEPS)