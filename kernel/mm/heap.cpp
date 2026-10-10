#include "mm/heap.h"

void *operator new(size_t size)           { return Heap::alloc(size); }
void  operator delete(void *ptr) noexcept { Heap::free(ptr); }

static VirtAddress HeapStart = 0;
static VirtAddress HeapEnd   = 0;
blockmeta_t *free_list = nullptr;

void Heap::init(VirtAddress start, size_t size) {
    if (size > HEAP_MAX_PAGES) size = HEAP_MAX_PAGES;
    PhysAddress HeapPhysBase = PMM::alloc_frames(size);
    if (HeapPhysBase == 0) __asm__ volatile ("ud2"); // Kernel Panic - Not syncing: Invalid Opcode
    if (!VMM::map_range(start, HeapPhysBase, size, VMM::Flags::Writable | VMM::Flags::NoExecute)) {
        PMM::free_frames(HeapPhysBase, size);
        __asm__ volatile ("ud2"); // Kernel Panic - Not syncing: Invalid Opcode
    }
    HeapStart = start;
    HeapEnd = start + (size * PMM::FRAME_SIZE);

    // block 0
    free_list = (blockmeta_t*)HeapStart;
    free_list->size = (HeapEnd - HeapStart) - META_SIZE;
    free_list->free = true;
    free_list->next = nullptr;
}

bool grow(size_t bytes) {
    size_t pages = ALIGN_UP(bytes, 16) / PMM::FRAME_SIZE;
    if (HeapEnd + pages * PMM::FRAME_SIZE > HeapStart + Heap::HEAP_MAX) return false; // too big

    PhysAddress physNewCompound = PMM::alloc_frames(pages);
    if (physNewCompound == 0) return false;

    if (!VMM::map_range(HeapEnd, physNewCompound, pages, VMM::Flags::Writable | VMM::Flags::NoExecute)) {
        PMM::free_frames(physNewCompound, pages);
        return false;
    }

    // adds new free block at the new heap section
    blockmeta_t *block = (blockmeta_t*)HeapEnd;
    block->free = true;
    block->size = pages * PMM::FRAME_SIZE - META_SIZE;
    block->next = nullptr;

    blockmeta_t *last = free_list;
    while (last->next != nullptr) last = last->next;
    last->next = block;

    HeapEnd += pages * PMM::FRAME_SIZE;
    return true;
}

void split_block(blockmeta_t *block, size_t size) {
    if (block->size < size + META_SIZE + 16) return;
    blockmeta_t *next = (blockmeta_t*)((uint8_t*)block + META_SIZE + size);

    next->size = block->size - size - META_SIZE;
    next->free = true;
    next->next = block->next;

    block->size = size;
    block->next = next;
}

void *Heap::alloc(size_t size) {
    if (size == 0) return NULL;
    if (size > HEAP_MAX) return NULL;

    size = ALIGN_UP(size, 16);

    for (blockmeta_t *block = free_list; block != nullptr; block = block->next) {
        if (!block->free || block->size < size) continue;
        split_block(block, size);
        block->free = false;
        return (void*)(block+1);
    }

    if (grow(size + META_SIZE)) return alloc(size);
    return nullptr;
}

void *Heap::calloc(size_t count, size_t size) {
    if (size != 0 && count > HEAP_MAX / size) return nullptr; // count * size > HEAP_MAX
    void *ptr = alloc(count * size);
    if (ptr != nullptr) Mem::memset(ptr, 0, count * size);
    return ptr;
}

void *Heap::realloc(void *ptr, size_t size) {
    blockmeta_t *block = (blockmeta_t*)ptr - 1;
    void *newptr = alloc(size);
    if (newptr == nullptr) return nullptr;
    Mem::memcpy(newptr, ptr, block->size);
    free(ptr);
    return newptr;
}

void Heap::free(void *ptr) {
    if (ptr == nullptr) return;
    blockmeta_t *block = (blockmeta_t*)ptr - 1;
    block->free = true;

    // cleanup
    for (blockmeta_t *b = free_list; b != nullptr && b->next != nullptr; ) {
        if (b->free && b->next->free) {
            b->size += META_SIZE + b->next->size; // the header of the next block becomes data
            b->next  = b->next->next;
        } else b = b->next;
    }
}