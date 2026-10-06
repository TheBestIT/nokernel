#ifndef COMMON_MEMORY_H
#define COMMON_MEMORY_H

#include <stddef.h>
#include <stdint.h>

using PhysAddress = uint64_t;   // An address in RAM or device memory, before paging.
using VirtAddress = uintptr_t;  // An address that the CPU translates through the page tables.

namespace Mem {
    #ifdef __cplusplus
    extern "C" {
    #endif

    /* ------------------ */
    // These implementations are in the public domain.
    // The compiler can make calls to memcpy, memset and memmove by itself.
    // Those calls use the C names, so these functions need C linkage.
    void  bcopy(const void *src, void *dest, size_t length);
    void *memcpy(void *out, const void *in, size_t length);
    void *memset(void *dest, int val, size_t length); 
    void *memmove(void *s1, const void *s2, size_t count);
    int   memcmp(const void *str1, const void *str2, size_t count);
    /* ------------------ */

    void *uint32_memset(void *dest, uint32_t val, size_t length);

    #ifdef __cplusplus
    }
    #endif
}



#endif // COMMON_MEMORY_H