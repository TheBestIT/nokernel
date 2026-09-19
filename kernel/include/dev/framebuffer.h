#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "shared/boot/info.h"
#include "shared/boot/framebuffer.h"

#include "lib/include/common/memory.h"

class Framebuffer {
    public:
        Framebuffer(bootinfo_t *bootinfo);
        void draw(uint32_t x, uint32_t y, uint32_t RGBD);
        void fill(uint32_t RGBD);
        
        uint32_t getHeight();
        uint32_t getWidth();
    private:
        framebuffer_t *fbDescriptor = nullptr;
        uint32_t *fb = nullptr;
};

#endif // FRAMEBUFFER_H