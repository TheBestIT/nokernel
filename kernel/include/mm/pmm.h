#ifndef PMM_H
#define PMM_H

#include "lib/common/include/memory.h"
#include "shared/boot/efimdesc.h"
#include "shared/boot/info.h"
#include "lib/common/include/stdlib.h"
#include "lib/common/include/math.h"

#define ALLOCATABLE_MEMORY_TYPES 1

// Physical Memory Manager
namespace PMM {
    constexpr size_t FRAME_SIZE = 4096;
    constexpr EFI_MEMORY_TYPE AllocatableMemoryTypes[ALLOCATABLE_MEMORY_TYPES] = { 
        // EFI_MEMORY_TYPE::EfiLoaderCode,
        // EFI_MEMORY_TYPE::EfiLoaderData,
        // EFI_MEMORY_TYPE::EfiBootServicesCode,
        // EFI_MEMORY_TYPE::EfiBootServicesData,
        EFI_MEMORY_TYPE::EfiConventionalMemory
    };

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