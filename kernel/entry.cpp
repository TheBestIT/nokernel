#include <stdint.h>

#include "shared/boot/info.h"

#include "include/dev/framebuffer.h"
#include "include/lib/font.h"
#include "include/console.h"

#define COM1 0x3F8
#define KERNEL_VERSION "0.0.1-PRE"

extern "C" [[noreturn]] void _start(bootinfo_t *bootInfo) {
    auto outb = [](uint16_t p, uint8_t v) {
        __asm__ volatile ("outb %0,%1" :: "a"(v), "Nd"(p));
    };
    for (const char *s = "kernel._start(...)\r\n"; *s; ++s) outb(COM1, *s);

    // init framebuffer
    Framebuffer fb = Framebuffer(bootInfo);
    font_t font = buildFontStruct(&bootInfo->font, 16, 32);
    Console console = Console(&fb, font);

    console.clear();
    console.setFGColor(0xFF00FF00);
    kprintf(console, "noKernel version %s\n\n", KERNEL_VERSION);
    console.setFGColor(0xFFFFFFFF);
    kprintf(console, "The framebuffer is loaded at %#x. Screen size is %ix%i\n", bootInfo->fb.base, bootInfo->fb.width, bootInfo->fb.height);
    kprintf(console, "Current loaded font is %ix%i.\nFont file loaded by EFI loader at base address %#x\n", font.CellWidth, font.CellHeight, bootInfo->font.BaseAddress);

    for (;;) __asm__ volatile ("hlt");
}
