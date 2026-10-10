#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include "lib/common/include/memory.h"
#include "shared/boot/info.h"

#include "pmm.h"

// Virtual Memory Manager
namespace VMM {
    enum Flags : uint64_t {
        Present   = 1ULL << 0,
        Writable  = 1ULL << 1,
        User      = 1ULL << 2,
        NoExecute = 1ULL << 63
    };

    constexpr uint64_t HHDM_OFFSET = 0xFFFF800000000000;
    constexpr uint64_t ADDR_MASK   = 0x000FFFFFFFFFF000; // wipes bit 0..11 and 52..63

    // pml4Phys holds the Table with the information about a page
    // including the physical frame address and the Flags
    static PhysAddress pml4Phys   = 0;
    static uint64_t    physOffset = 0;

    size_t index(VirtAddress virt, int level);

    void init(const bootinfo_t *bootInfo);

    bool map(VirtAddress virt, PhysAddress phys, uint64_t flags); // maps one 4KiB page
    bool map_range(VirtAddress virt, PhysAddress phys, size_t count, uint64_t flags);

    void unmap(VirtAddress virt); // clears the entry and calls upon invlpg
    void unmap_range(VirtAddress virt, size_t count);

    PhysAddress translate(VirtAddress virt); // returns 0 if the page has no entry
    void *phys_to_virt(PhysAddress phys); // gives access to a frame
}

#endif // VMM_H