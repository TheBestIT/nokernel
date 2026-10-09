#include "mm/pmm.h"

static uint8_t *bitmap      = nullptr; // holds the state of the frames (FREE, USED)
static size_t   totalFrames = 0; // 1 frame is PMM::FRAME_SIZE bytes
static size_t   freeFrames  = 0;

static void clear(size_t f) { bitmap[f / 8] &= ~(1 << (f % 8)); }
static void set(size_t f) { bitmap[f / 8] |= (1 << (f % 8)); }
static bool state(size_t f) { return (bool)(bitmap[f / 8] & (1 << (f % 8))); }

static EFI_MEMORY_DESCRIPTOR *mementry(const bootinfo_t *bootInfo, uint64_t offset) {
    return (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)bootInfo->mmap + offset);
}

size_t PMM::total_count() { return totalFrames; }
size_t PMM::free_count() { return freeFrames; }

void PMM::reserve(PhysAddress first, size_t count) {
    size_t frameIndex = first / PMM::FRAME_SIZE;
    if (frameIndex >= totalFrames || count > totalFrames - frameIndex) return;
    for (size_t i = 0; i < count; i++) {
        if (!state(frameIndex + i)) freeFrames--; 
        set(frameIndex + i);
    }
}

void PMM::init(const bootinfo_t *bootInfo) {
    // indexing over the memory map...
    PhysAddress highest = 0;
    for (uint64_t offset = 0; offset < bootInfo->mmap_size; offset += bootInfo->desc_size) {
        auto *descriptor = mementry(bootInfo, offset);
        PhysAddress start = descriptor->PhysicalStart;
        PhysAddress end   = start + descriptor->NumberOfPages * PMM::FRAME_SIZE;
        auto type = (EFI_MEMORY_TYPE)descriptor->Type;
        
        if (contains(type, PMM::AllocatableMemoryTypes, ALLOCATABLE_MEMORY_TYPES)) {
            if (end > highest) highest = end;
        }
    }

    totalFrames = highest / PMM::FRAME_SIZE; // converts the size of the highest frame to frames
    size_t bitmapBytes  = ALIGN_UP(totalFrames, 8) / 8; // converts the frames (bits, aligned to a base of 8 [1 byte]) to bytes
    size_t bitmapFrames = ALIGN_UP(bitmapBytes, PMM::FRAME_SIZE) / FRAME_SIZE; // convert back the bitmapBytes to frames to allocate them 

    for (uint64_t offset = 0; offset < bootInfo->mmap_size; offset += bootInfo->desc_size) {
        auto *descriptor = mementry(bootInfo, offset);
        if (descriptor->Type == EFI_MEMORY_TYPE::EfiConventionalMemory && descriptor->PhysicalStart != 0 && descriptor->NumberOfPages >= bitmapFrames) {
            bitmap = (uint8_t*)descriptor->PhysicalStart;
            break;
        }
    }

    if (bitmap == nullptr) __asm__ volatile ("ud2"); // Kernel Panic - Not Syncing: Invalid Opcode

    Mem::memset(bitmap, 0xFF, bitmapBytes); // sets all frames as used

    for (uint64_t offset = 0; offset < bootInfo->mmap_size; offset += bootInfo->desc_size) {
        auto *descriptor = mementry(bootInfo, offset);
        auto type = (EFI_MEMORY_TYPE)descriptor->Type;
        if (!contains(type, PMM::AllocatableMemoryTypes, ALLOCATABLE_MEMORY_TYPES)) continue;
        size_t first = descriptor->PhysicalStart / PMM::FRAME_SIZE;
        for (size_t f = first; f < first + descriptor->NumberOfPages; f++) { clear(f); freeFrames++; }
    }

    reserve((PhysAddress)bitmap, bitmapFrames);
    reserve(0, 1);

    kprintf("Allocated the Memory Bitmap at address: %#lx.\nTotal Frames: %lu; Free: %lu; Used: %lu (%lu%%)\n", (PhysAddress)bitmap, totalFrames, freeFrames, totalFrames-freeFrames, (totalFrames-freeFrames) * 100 / totalFrames);
}



// Search the first free frame in the bitmap
PhysAddress PMM::alloc_frame() { return alloc_frames(1); }

PhysAddress PMM::alloc_frames(size_t count) {
    if (count == 0 || count > freeFrames) return 0;

    size_t run = 0;
    for (size_t frame = 0; frame < totalFrames; frame++) {
        run = state(frame) ? 0 : run + 1;
        if (run < count) continue;

        size_t first = frame + 1 - count;
        reserve(first * PMM::FRAME_SIZE, count);
        return first * PMM::FRAME_SIZE;
    }
    return 0;
}

void PMM::free_frame(PhysAddress frame) {
    if (freeFrames == totalFrames) return;

    size_t frameIndex = frame / PMM::FRAME_SIZE;

    if (frameIndex == 0 || frameIndex >= totalFrames) return;

    if (!state(frameIndex)) return;
    clear(frameIndex);
    freeFrames++;
}

void PMM::free_frames(PhysAddress first, size_t count) {
    size_t firstIndex = first / PMM::FRAME_SIZE;

    if (firstIndex >= totalFrames || count > totalFrames - firstIndex) return;

    for (size_t i = 0; i < count; i++) PMM::free_frame((firstIndex + i) * PMM::FRAME_SIZE);
}