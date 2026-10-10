#include <stdint.h>

#include "shared/boot/info.h"

#include "include/dev/framebuffer.h"
#include "include/lib/font.h"
#include "include/dev/console.h"
#include "include/arch/x86_64/arch.h"

#include "include/mm/pmm.h"
#include "include/mm/vmm.h"
#include "include/mm/heap.h"
#include "include/panic.h"

#define KERNEL_VERSION "0.1.1-PRE"

void HeapAlloc() {
    size_t HeapInitSize = 0ULL;
    size_t targetSize = 256ULL;
    do {
        if (targetSize == 1ULL) break;
        if (PMM::free_count() < targetSize) targetSize /= 2ULL;
        else HeapInitSize = targetSize;
    } while (HeapInitSize == 0ULL);

    if (HeapInitSize == 0ULL) PANIC("Out of Memory. Can't establish HeapInitSize. Free Memory: %d bytes", PMM::free_count() * PMM::FRAME_SIZE);

    VirtAddress HeapStart = 0xFFFFC00000000000;
    Heap::init(HeapStart, HeapInitSize);
}

extern "C" [[noreturn]] void _start(bootinfo_t *bootInfo) {
    x86_64::ArchInit(); // Inits GDT, IDT, IRQs, ISRs
    PMM::init(bootInfo);
    VMM::init(bootInfo);
    HeapAlloc(); // Heap!

    auto *fb = new Framebuffer(&bootInfo->fb);
    font_t font = buildFontStruct(&bootInfo->font, 16, 32);
    console_init(fb, font);

    console().clear();
    console().setFGColor(0xFF00FF00);
    kprintf("noKernel version %s\n\n", KERNEL_VERSION);
    console().setFGColor(0xFFFFFFFF);
    kprintf("The framebuffer is loaded at %#x. Screen size is %ix%i\n", bootInfo->fb.base, bootInfo->fb.width, bootInfo->fb.height);
    kprintf("Current loaded font is %ix%i.\nFont file loaded by EFI loader at base address %#x\n", font.CellWidth, font.CellHeight, bootInfo->font.BaseAddress);

    uint16_t cs;
    __asm__ volatile("movw %%cs, %0" : "=r"(cs)); // moves the content of %cs to cs
    kprintf("The index of the 'cs' register is %#x\n", cs);

    for (;;) __asm__ volatile ("hlt");
}
