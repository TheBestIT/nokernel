#ifndef SHARED_BOOT_FRAMEBUFFER_H
#define SHARED_BOOT_FRAMEBUFFER_H

#include <stdint.h>

typedef struct {
    uint64_t base;
    uint64_t size;
    uint32_t width, height, pitch, format;
} framebuffer_t;

#endif // SHARED_BOOT_FRAMEBUFFER_H