#include "kernel/include/console.h"

Console::Console(Framebuffer *fb, font_t font) {
    this->fb = fb;
    this->font = font;
}

void Console::newline() {
    this->x = 0;
    this->y += this->font.CellHeight;
    if (this->y / this->font.CellHeight > this->fb->getHeight() / this->font.CellHeight) this->clear();
}

void Console::print_char(char c) {
    if (c == '\n') return this->newline();
    
    uint32_t col = (uint8_t)c % this->font.CellsPerXAxis;
    uint32_t row = (uint8_t)c / this->font.CellsPerXAxis;

    uint32_t x0 = col * this->font.CellWidth;
    uint32_t y0 = row * this->font.CellHeight;

    uint8_t *pixels = this->font.BMPDataSegmentOffsetted;

    for (uint32_t gy = 0; gy < this->font.CellHeight; gy++) {
        uint32_t imageRow = y0 + gy;
        uint32_t fileRow  = this->font.BMPImageHeight - 1 - imageRow;
        uint8_t *rowStart = pixels + (uint64_t)fileRow * this->font.Stride
                                   + (uint64_t)x0 * this->font.BitsPerPixel / 8;
        
        uint16_t bits = (uint16_t)((rowStart[0] << 8) | rowStart[1]);
        for (uint32_t gx = 0; gx < this->font.CellWidth; gx++)
            if (bits >> (15 - gx) & 1) this->fb->draw(gx + this->x, gy + this->y, this->foregroundColor);
    }

    this->x += this->font.CellWidth;
    if (this->x / this->font.CellWidth > this->fb->getWidth() / this->font.CellWidth) this->newline();
    
}

void Console::clear() {
    this->x = 0;
    this->y = 0;
    this->fb->fill(this->backgroundColor);
}

void Console::print(const char *str) {
    for (uint32_t i = 0; str[i]; i++)
        this->print_char(str[i]);
}

void Console::setBGColor(uint32_t color) {
    this->backgroundColor = color;
}

void Console::setFGColor(uint32_t color) {
    this->foregroundColor = color;
}

static void conout(char c, void *arg) {
    ((Console *)arg)->print_char(c);
}

int kprintf(Console &console, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = _vfctprintf(conout, &console, format, args);
    va_end(args);
    return ret;
}