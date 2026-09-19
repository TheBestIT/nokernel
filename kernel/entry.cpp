#include <stdint.h>

extern "C" [[noreturn]] void _start()
{
    auto outb = [](uint16_t p, uint8_t v) {
        __asm__ volatile ("outb %0,%1" :: "a"(v), "Nd"(p));
    };
    for (const char *s = "kernel alive\r\n"; *s; ++s) outb(0x3F8, *s);
    for (;;) __asm__ volatile ("hlt");
}
