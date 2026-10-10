#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "lib/common/include/memory.h"
#include "vmm.h"
#include "pmm.h"

struct blockmeta_t {
    size_t size;
    blockmeta_t *next;
    bool free;
};
const size_t META_SIZE = sizeof(blockmeta_t);

namespace Heap {
    constexpr size_t HEAP_MAX       = 1 << 30ULL; // 1GiB
    constexpr size_t HEAP_MAX_PAGES = HEAP_MAX >> 12;
    constexpr size_t HEAP_STEP      = 1 << 20ULL; // 1MiB

    void  init(VirtAddress start, size_t size);
    void *alloc(size_t size);
    void *calloc(size_t count, size_t size);
    void *realloc(void* ptr, size_t size);
    void  free(void *ptr);
}

#endif // HEAP_H