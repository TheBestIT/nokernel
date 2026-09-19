#include "dev/framebuffer.h"

Framebuffer::Framebuffer(bootinfo_t *bootinfo) {
    this->fbDescriptor = &bootinfo->fb;
    this->fb = (uint32_t*)bootinfo->fb.base;
}

void Framebuffer::draw(uint32_t x, uint32_t y, uint32_t RGBD) {
    uint32_t pixelOffset = (y * this->fbDescriptor->pitch) + x;
    this->fb[pixelOffset] = RGBD;
}

void Framebuffer::fill(uint32_t RGBD) {
    uint32_memset(this->fb, RGBD, this->fbDescriptor->size);
}

uint32_t Framebuffer::getHeight() {
    return this->fbDescriptor->height;
}

uint32_t Framebuffer::getWidth() {
    return this->fbDescriptor->width;
}