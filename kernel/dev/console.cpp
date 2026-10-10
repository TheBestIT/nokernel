#include "kernel/include/dev/console.h"

static Console *s_console = nullptr;
bool ROUTE_TO_COM = true;

Console::Console(Framebuffer *fb, font_t font) {
    this->fb = fb;
    this->font = font;

    // alloc shadow framebuffer for screen scrolling
    this->fbShadow = fb->clone();
}

void Console::scroll() {
    size_t offset = fb->getDescriptor()->pitch * this->font.CellHeight * 4; // 1 screen line with the current font
    this->fb->copy(this->fbShadow, offset);
    this->fb->fill(this->backgroundColor);
    this->fbShadow->copy(this->fb, 0);
    this->y -= this->font.CellHeight;
}

void Console::newline() {
    this->x = 0;
    if ((this->y + this->font.CellHeight) / this->font.CellHeight > this->fb->getHeight() / this->font.CellHeight) return this->scroll();
    this->y += this->font.CellHeight;
}

void Console::print_char(char c) {
    if (c == '\n') return this->newline();

    if ((this->y + this->font.CellHeight) / this->font.CellHeight > this->fb->getHeight() / this->font.CellHeight) {
        this->scroll();
    }
    
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
    this->fbShadow->fill(this->backgroundColor);
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

void console_init(Framebuffer *fb, font_t font) {
    if (s_console != nullptr) return;
    s_console = new Console(fb, font);
}

Console &console() {
    return *s_console;
}

void serialout(char v, void *arg) {
    (void)arg;
    __asm__ volatile ("outb %0,%1" :: "a"((uint8_t)v), "Nd"((uint16_t)COM1));
};

static void conout(char c, void *arg) {
    if (ROUTE_TO_COM) serialout(c, NULL);
    ((Console *)arg)->print_char(c);
}

int kprintf(Console &console, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = _vfctprintf(conout, &console, format, args);
    va_end(args);
    return ret;
}

int kprintf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = _vfctprintf(conout, &console(), format, args);
    va_end(args);
    return ret;
}

int kvprintf(const char *format, va_list args) {
    return _vfctprintf(conout, &console(), format, args);
}