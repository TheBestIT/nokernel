#ifndef CONSOLE_H
#define CONSOLE_H

#include "kernel/include/dev/framebuffer.h"
#include "kernel/include/lib/font.h"

class Console {
    public:
        Console(Framebuffer *fb, font_t font);
        void clear();
        void print(const char *str);
        void setBGColor(uint32_t color);
        void setFGColor(uint32_t color);
    private:
        void print_char(char c);

        Framebuffer *fb;
        font_t font;

        uint32_t backgroundColor = 0x00000000;
        uint32_t foregroundColor = 0xFFFFFFFF;
        uint32_t x = 0;
        uint32_t y = 0;
};

#endif // CONSOLE_H