#ifndef CONSOLE_H
#define CONSOLE_H

#include "framebuffer.h"
#include "lib/font.h"
#include "lib/common/include/vsnprintf.h"
#include "mm/heap.h"

#define COM1 0x3F8

extern bool ROUTE_TO_COM;

class Console {
    public:
        Console(Framebuffer *fb, font_t font);
        void clear();
        void print(const char *str);
        void print_char(char c);
        void setBGColor(uint32_t color);
        void setFGColor(uint32_t color);
    private:
        void newline();
        void scroll();
        Framebuffer *fb;
        Framebuffer *fbShadow;
        font_t font;

        uint32_t backgroundColor = 0x00000000;
        uint32_t foregroundColor = 0xFFFFFFFF;
        uint32_t x = 0;
        uint32_t y = 0;
};

void serialout(char v, void *arg);

void console_init(Framebuffer *fb, font_t font);
Console &console();

int kprintf(Console &console, const char *format, ...);
int kprintf(const char *format, ...);
int kvprintf(const char *format, va_list args);

#endif // CONSOLE_H