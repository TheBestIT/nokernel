#include "mm/vmm.h"
#include "panic.h"

size_t VMM::index(VirtAddress virt, int level) {
    // gets the virtual address and proceeds with those steps:
    // bitshifts to the right by 12 (to remove the extra zeros at the end of the virtual address)
    // bitshifts to the right by (9 * level), level goes from 0 to 3 and indicates 4 types of pages:
    // (level=0, size=4KiB), (level=1, size=2MiB), (level=2, size=1GiB), (level=3, size=512GiB)
    // all of those derive from bitshifting by 9 to the left (basically multiplying by 512)
    // ANDs by 0x1FF, which removes the HHDM_OFFSET map leaving just the bare frame index.
    return (virt >> (12 + 9 * level)) & 0x1FF;
};

void *VMM::phys_to_virt(PhysAddress phys) {
    return (void *)(phys + VMM::physOffset);
}

// Reads the Extended Feature Enable Register (0xC0000080) from the Model Specific Register
// and flips the 11th bit to a 1 to protect non-executable memory pages
void enable_nx() {
    uint32_t lo, hi;
    __asm__ volatile ("rdmsr" : "=a"(lo), "=d"(hi) : "c"(0xC0000080));
    lo |= 1 << 11;
    __asm__ volatile ("wrmsr" :: "a"(lo), "d"(hi), "c"(0xC0000080));
}

static uint64_t *next_table(uint64_t *table, size_t index, bool create) {
    if (!(table[index] & VMM::Flags::Present)) {
        if (!create) return nullptr; // if it doesn't exist and create is false, returns
        PhysAddress frame = PMM::alloc_frame();
        if (frame == 0) return nullptr;
        Mem::memset(VMM::phys_to_virt(frame), 0, PMM::FRAME_SIZE);
        table[index] = frame | VMM::Flags::Present | VMM::Flags::Writable; 
    }
    return (uint64_t*)VMM::phys_to_virt(table[index] & VMM::ADDR_MASK); // returns the virtual address of the table without any flags (ADDR_MASK)
}

// Finds the virtual address pml4Phys table entry with its physical address and flags
// Can create the entry if it doesn't exist in the pml4Phys table
static uint64_t *find_entry(VirtAddress virt, bool create) {
    uint64_t *table = (uint64_t*)VMM::phys_to_virt(VMM::pml4Phys);
    for (int level = 3; level > 0; level--) {
        table = next_table(table, VMM::index(virt, level), create);
        if (table == nullptr) return nullptr;
    }

    return &table[VMM::index(virt, 0)];
}

void VMM::unmap(VirtAddress virt) {
    auto entry = find_entry(virt, false);
    if (entry == nullptr) return;
    *entry = 0;
    __asm__ volatile ("invlpg (%0)" :: "r"(virt) : "memory");
}

void VMM::unmap_range(VirtAddress virt, size_t count) {
    for (size_t i = 0; i < count; i++, virt += PMM::FRAME_SIZE) unmap(virt);
}

bool VMM::map(VirtAddress virt, PhysAddress phys, uint64_t flags) {
    auto entry = find_entry(virt, true);
    if (entry == nullptr || (*entry & Flags::Present)) return false;
    *entry = (phys & VMM::ADDR_MASK) | flags | Flags::Present;
    return true;
}

bool VMM::map_range(VirtAddress virt, PhysAddress phys, size_t count, uint64_t flags) {
    for (size_t i = 0; i < count; i++, phys += PMM::FRAME_SIZE) {
        if (map(virt + i * PMM::FRAME_SIZE, phys, flags)) continue;
        unmap_range(virt, i);
        return false;
    }
    return true;
}

PhysAddress VMM::translate(VirtAddress virt) {
    auto entry = find_entry(virt, false);
    if (entry == nullptr || !(*entry & Present)) return 0;
    return (*entry & ADDR_MASK) + (virt & 0xFFF);
}

void map_both(PhysAddress phys, size_t count) {
    if (phys != 0) VMM::map_range(phys, phys, count, VMM::Flags::Writable); // identity map
    VMM::map_range(phys + VMM::HHDM_OFFSET, phys, count, VMM::Flags::Writable); // direct map
}

void VMM::init(const bootinfo_t *bootInfo) {
    enable_nx();

    pml4Phys = PMM::alloc_frame();
    ASSERT(pml4Phys != 0);
    Mem::memset(phys_to_virt(pml4Phys), 0, PMM::FRAME_SIZE);

    for (uint64_t offset = 0; offset < bootInfo->mmap_size; offset += bootInfo->desc_size) {
        auto *descriptor = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)bootInfo->mmap + offset);
        map_both(descriptor->PhysicalStart, descriptor->NumberOfPages);
    }

    size_t fbPages = ALIGN_UP(bootInfo->fb.size, PMM::FRAME_SIZE) / PMM::FRAME_SIZE; // pages of the framebuffer
    map_both(bootInfo->fb.base, fbPages);

    __asm__ volatile ("mov %0, %%cr3" :: "r"(pml4Phys) : "memory"); // gives %cr3 the address of the Page Map Level 4 (pml4)
    physOffset = HHDM_OFFSET;
}