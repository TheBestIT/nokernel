#ifndef PMM_H
#define PMM_H

#include "lib/common/include/memory.h"
#include "shared/boot/info.h"

// Physical Memory Manager
namespace PMM {
    constexpr size_t FRAME_SIZE = 4096;

    void init(const bootinfo_t *bootInfo);

    PhysAddress alloc_frame();
    PhysAddress alloc_frames(size_t count);

    void free_frame(PhysAddress frame);
    void free_frames(PhysAddress first, size_t count);
    void reserve(PhysAddress first, size_t count);
    size_t total_count();
    size_t free_count();
}

#endif // PMM_H