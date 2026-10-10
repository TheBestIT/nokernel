#include "dev/framebuffer.h"
#include "panic.h"

Framebuffer::Framebuffer(framebuffer_t *fbDescriptor) {
    this->fbDescriptor = fbDescriptor;
    // The UEFI page tables map memory 1:1, so the physical address is also the virtual address.
    PhysAddress base = fbDescriptor->base;
    this->fb = (uint32_t*)(VirtAddress)base;
}

void Framebuffer::draw(uint32_t x, uint32_t y, uint32_t RGBD) {
    uint32_t pixelOffset = (y * this->fbDescriptor->pitch) + x;
    this->fb[pixelOffset] = RGBD;
}

void Framebuffer::fill(uint32_t RGBD) {
    Mem::uint32_memset(this->fb, RGBD, this->fbDescriptor->size / 4);
}

uint32_t Framebuffer::getHeight() {
    return this->fbDescriptor->height;
}

uint32_t Framebuffer::getWidth() {
    return this->fbDescriptor->width;
}

bool Framebuffer::copy(Framebuffer *destination, size_t offset) {
    framebuffer_t *destinationDescriptor = destination->getDescriptor();

    if (destinationDescriptor->size < this->fbDescriptor->size - offset) return false; // copying over would result in a OOB write

    Mem::memcpy((void*)destinationDescriptor->base, (void*)(this->fbDescriptor->base + offset), this->fbDescriptor->size - offset);
    return true;
}

// Returns a new allocated *fb with the same sizes as the current fb but its content allocated to 0
Framebuffer *Framebuffer::clone() {
    framebuffer_t *clonedFbDescriptor = new framebuffer_t;
    clonedFbDescriptor->format = this->fbDescriptor->format;
    clonedFbDescriptor->pitch = this->fbDescriptor->pitch;
    clonedFbDescriptor->height = this->fbDescriptor->height;
    clonedFbDescriptor->width = this->fbDescriptor->width;
    clonedFbDescriptor->size = this->fbDescriptor->size;

    void *ptr = Heap::alloc(clonedFbDescriptor->size);
    ASSERT(ptr != nullptr);
    Mem::memset(ptr, 0, clonedFbDescriptor->size);
    clonedFbDescriptor->base = (VirtAddress)ptr;

    return new Framebuffer(clonedFbDescriptor); // new already asserts ptr != nullptr
}

framebuffer_t *Framebuffer::getDescriptor() {
    return this->fbDescriptor;
}