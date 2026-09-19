#include <stdint.h>

#include "shared/boot/info.h"

#include "include/dev/framebuffer.h"
#include "include/lib/font.h"
#include "include/console.h"

#define COM1 0x3F8

extern "C" [[noreturn]] void _start(bootinfo_t *bootInfo) {
    auto outb = [](uint16_t p, uint8_t v) {
        __asm__ volatile ("outb %0,%1" :: "a"(v), "Nd"(p));
    };
    for (const char *s = "kernel alive\r\n"; *s; ++s) outb(COM1, *s);

    // init framebuffer
    Framebuffer fb = Framebuffer(bootInfo);
    font_t font = buildFontStruct(&bootInfo->font, 16, 32);
    Console console = Console(&fb, font);

    console.clear();
    console.print("Hello, kernel space!");

    for (;;) __asm__ volatile ("hlt");
}
