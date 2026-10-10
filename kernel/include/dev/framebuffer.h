#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "shared/boot/info.h"
#include "shared/boot/framebuffer.h"

#include "lib/common/include/memory.h"
#include "mm/heap.h"

class Framebuffer {
    public:
        Framebuffer(framebuffer_t *fbDescriptor);
        void draw(uint32_t x, uint32_t y, uint32_t RGBD);
        void fill(uint32_t RGBD);
        
        uint32_t getHeight();
        uint32_t getWidth();
        bool copy(Framebuffer *destination, size_t offset);
        Framebuffer *clone();

        framebuffer_t *getDescriptor();
    private:
        framebuffer_t *fbDescriptor = nullptr;
        uint32_t *fb = nullptr;
};

#endif // FRAMEBUFFER_H