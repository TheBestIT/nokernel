CLANG := clang
LLD   := $(shell command -v lld-link 2>/dev/null)

BUILD := build
ESP   := $(BUILD)/esp

EFI_INCLUDES := -I boot/src/include -I boot/src -I ./

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

CLANGXX := clang++
LDLLD   := ld.lld


KERNEL_INCLUDES := -I ./ -I kernel/include
KERNEL_CXXFLAGS := -target x86_64-unknown-elf -std=c++20 \
                   -ffreestanding -fno-exceptions -fno-rtti \
                   -fno-stack-protector -fno-pic -mno-red-zone \
                   -mgeneral-regs-only -nostdlib -DCOMMON_NO_FLOAT \
                   -Wall -Wextra -MMD -MP $(KERNEL_INCLUDES)

KERNEL_LDSCRIPT := kernel/linker.ld
KERNEL_LDFLAGS  := -nostdlib -static -T $(KERNEL_LDSCRIPT)

KERNEL_ASFLAGS := -target x86_64-unknown-elf -MMD -MP $(KERNEL_INCLUDES)

KERNEL_SRCS := $(shell find kernel -name '*.cpp')
KERNEL_ASM  := $(shell find kernel -name '*.S')
KERNEL_OBJS := $(patsubst kernel/%.cpp,$(BUILD)/kernel/%.o,$(KERNEL_SRCS)) \
               $(patsubst kernel/%.S,$(BUILD)/kernel/%.S.o,$(KERNEL_ASM))
KERNEL_DEPS := $(KERNEL_OBJS:.o=.d)
KERNEL_ELF  := $(BUILD)/kernel.elf

# Shared library code. It links into the kernel with the kernel flags.
LIB_SRCS := $(shell find lib -name '*.cpp')
LIB_OBJS := $(patsubst lib/%.cpp,$(BUILD)/lib/%.o,$(LIB_SRCS))
LIB_DEPS := $(LIB_OBJS:.o=.d)

IMG := $(BUILD)/loader.img

.PHONY: all esp image run debug run-esp debug-esp run-img debug-img \
        clean distclean ovmf bootx64 kernel

all: esp image $(OVMF_CODE)

esp:   $(ESP)/EFI/BOOT/BOOTX64.EFI $(ESP)/kernel.elf
image: $(IMG)

bootx64: $(BUILD)/BOOTX64.EFI
ovmf:    $(OVMF_CODE)
kernel:  $(KERNEL_ELF)

# Boot from the build/esp folder.
run-esp: esp $(OVMF_CODE)
	bash tools/start_qemu.sh esp

debug-esp: esp $(OVMF_CODE)
	bash tools/start_qemu.sh esp -d

# Boot from the build/loader.img disk image.
run-img: image $(OVMF_CODE)
	bash tools/start_qemu.sh img

debug-img: image $(OVMF_CODE)
	bash tools/start_qemu.sh img -d

# Default: boot from the ESP folder.
run:   run-esp
debug: debug-esp

$(BUILD)/BOOTX64.EFI: $(EFI_OBJS)
	@test -n "$(LLD)" || { echo "lld-link non trovato: sudo apt install lld"; exit 1; }
	$(CLANG) $(EFI_CFLAGS) $(EFI_LDFLAGS) -o $@ $^

$(BUILD)/boot/%.o: boot/src/%.c
	@mkdir -p $(dir $@)
	$(CLANG) $(EFI_CFLAGS) -c -o $@ $<

$(KERNEL_ELF): $(KERNEL_OBJS) $(LIB_OBJS) $(KERNEL_LDSCRIPT)
	$(LDLLD) $(KERNEL_LDFLAGS) -o $@ $(KERNEL_OBJS) $(LIB_OBJS)

$(BUILD)/kernel/%.o: kernel/%.cpp
	@mkdir -p $(dir $@)
	$(CLANGXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(BUILD)/kernel/%.S.o: kernel/%.S
	@mkdir -p $(dir $@)
	$(CLANG) $(KERNEL_ASFLAGS) -c -o $@ $<

$(BUILD)/lib/%.o: lib/%.cpp
	@mkdir -p $(dir $@)
	$(CLANGXX) $(KERNEL_CXXFLAGS) -c -o $@ $<

$(ESP)/EFI/BOOT/BOOTX64.EFI: $(BUILD)/BOOTX64.EFI $(KERNEL_ELF)
	bash tools/prepare_esp.sh

$(ESP)/kernel.elf: $(KERNEL_ELF)
	@mkdir -p $(dir $@)
	cp $< $@

$(IMG): $(BUILD)/BOOTX64.EFI $(KERNEL_ELF)
	bash tools/build_img.sh

$(OVMF_CODE):
	bash tools/fetch_ovmf.sh

clean:
	rm -rf $(BUILD)/BOOTX64.EFI $(BUILD)/boot $(KERNEL_ELF) $(BUILD)/kernel $(BUILD)/lib $(ESP) $(IMG)

distclean: clean
	rm -rf $(BUILD)

-include $(EFI_DEPS) $(KERNEL_DEPS) $(LIB_DEPS)