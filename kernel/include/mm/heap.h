#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include "lib/common/include/memory.h"

namespace Heap {
    void  init(VirtAddress start, size_t size);
    void *alloc(size_t size);
    void *calloc(size_t count, size_t size);
    void *realloc(void* ptr, size_t size);
    void  free(void *ptr);
}

void *operator new(size_t size)           { return Heap::alloc(size); }
void  operator delete(void *ptr) noexcept { Heap::free(ptr); }

#endif // HEAP_H