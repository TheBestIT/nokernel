#include "panic.h"
#include "lib/common/include/vsnprintf.h"

int keprintf(bool serial, const char *format, ...) { // kernel emergency printf
    ROUTE_TO_COM = false;
    va_list args;
    va_start(args, format);
    int ret = 0;
    if (serial) ret = _vfctprintf(serialout, NULL, format, args);
    else ret = kvprintf(format, args);
    va_end(args);
    return ret;
}

int kveprintf(bool serial, const char *format, va_list args) {
    ROUTE_TO_COM = false;
    int ret = 0;
    if (serial) ret = _vfctprintf(serialout, NULL, format, args);
    else ret = kvprintf(format, args);
    return ret;
}

[[noreturn]] void halt() {
    for (;;) {
        __asm__ volatile ("cli");
        __asm__ volatile ("hlt");
    }
}

[[noreturn]] void panic(const char* file, int line, const char *fmt, ...) {
    __asm__ volatile ("cli");

    static bool panic = false;
    if (panic) halt();
    panic = true;

    for (int i = 1; i > -1; i--) {
        keprintf((bool)i, "\nKernel panic - Not Syncing: ");
        va_list args;
        va_start(args, fmt);
        kveprintf((bool)i, fmt, args);
        va_end(args);
        keprintf((bool)i, "\n  at %s:%d", file, line);
    }

    halt();
} 