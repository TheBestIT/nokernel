#ifndef PANIC_H
#define PANIC_H

#include "dev/console.h"

int keprintf(bool serial, const char *format, ...);
int kveprintf(bool serial, const char *format, va_list args);
[[noreturn]] void halt();

[[noreturn]] void panic(const char *file, int line, const char *fmt, ...);

#define PANIC(...) panic(__FILE__, __LINE__, __VA_ARGS__)
#define ASSERT(cond) \
    do { if (!(cond)) PANIC("Assertion failed: %s", #cond); } while (0)

#endif // PANIC_H