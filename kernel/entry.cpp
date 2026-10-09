#include <stdint.h>

#include "shared/boot/info.h"

#include "include/dev/framebuffer.h"
#include "include/lib/font.h"
#include "include/console.h"
#include "include/arch/x86_64/arch.h"

#include "include/mm/pmm.h"

#define COM1 0x3F8
#define KERNEL_VERSION "0.0.3-PRE"

extern "C" [[noreturn]] void _start(bootinfo_t *bootInfo) {
    auto outb = [](uint16_t p, uint8_t v) {
        __asm__ volatile ("outb %0,%1" :: "a"(v), "Nd"(p));
    };
    for (const char *s = "kernel._start(...)\r\n"; *s; ++s) outb(COM1, *s);

    // init framebuffer
    Framebuffer fb = Framebuffer(bootInfo);
    font_t font = buildFontStruct(&bootInfo->font, 16, 32);
    Console console = Console(&fb, font);
    g_console = &console; // global console

    x86_64::ArchInit(); // Inits GDT, IDT, IRQs, ISRs

    console.clear();
    console.setFGColor(0xFF00FF00);
    kprintf("noKernel version %s\n\n", KERNEL_VERSION);
    console.setFGColor(0xFFFFFFFF);
    kprintf("The framebuffer is loaded at %#x. Screen size is %ix%i\n", bootInfo->fb.base, bootInfo->fb.width, bootInfo->fb.height);
    kprintf("Current loaded font is %ix%i.\nFont file loaded by EFI loader at base address %#x\n", font.CellWidth, font.CellHeight, bootInfo->font.BaseAddress);

    uint16_t cs;
    __asm__ volatile("movw %%cs, %0" : "=r"(cs)); // moves the content of %cs to cs
    kprintf("The index of the 'cs' register is %#x\n", cs);

    PMM::init(bootInfo);

    size_t before = PMM::free_count();
    PhysAddress a = PMM::alloc_frame();
    PhysAddress b = PMM::alloc_frame();
    Mem::memset((void*)a, 0xFC, PMM::FRAME_SIZE);
    kprintf("a=%#lx b=%#lx free=%lu\n", a, b, PMM::free_count());

    PMM::free_frame(a);
    PhysAddress c = PMM::alloc_frame();
    PhysAddress d = PMM::alloc_frames(4);
    kprintf("c=%#lx d=%#lx\n", c, d);

    PMM::free_frames(d, 4);
    PMM::free_frame(b);
    PMM::free_frame(c);
    kprintf("free=%lu before=%lu\n", PMM::free_count(), before);

    for (;;) __asm__ volatile ("hlt");
}
