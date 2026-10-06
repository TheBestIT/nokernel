#include "dev/framebuffer.h"

Framebuffer::Framebuffer(bootinfo_t *bootinfo) {
    this->fbDescriptor = &bootinfo->fb;
    // The UEFI page tables map memory 1:1, so the physical address is also the virtual address.
    PhysAddress base = bootinfo->fb.base;
    this->fb = (uint32_t*)(VirtAddress)base;
}

void Framebuffer::draw(uint32_t x, uint32_t y, uint32_t RGBD) {
    uint32_t pixelOffset = (y * this->fbDescriptor->pitch) + x;
    this->fb[pixelOffset] = RGBD;
}

void Framebuffer::fill(uint32_t RGBD) {
    Mem::uint32_memset(this->fb, RGBD, this->fbDescriptor->size);
}

uint32_t Framebuffer::getHeight() {
    return this->fbDescriptor->height;
}

uint32_t Framebuffer::getWidth() {
    return this->fbDescriptor->width;
}