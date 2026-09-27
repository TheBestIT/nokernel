#include "include/memory.h"

void bcopy(const void *src, void *dest, size_t length) {
    if (dest < src) {
        const char *firsts = (const char *) src;
        char *firstd = (char *) dest;
        while (length--)
            *firstd++ = *firsts++;
    } else {
        const char *lasts = (const char *)src + (length-1);
        char *lastd = (char *) dest + (length-1);
        while (length--)
            *lastd-- = *lasts--;
    }
}

void *memcpy(void *out, const void *in, size_t length) {
    bcopy(in, out, length);
    return out;
}

void *memset(void *dest, int val, size_t length) {
    unsigned char *ptr = (unsigned char *)dest;
    while (length-- > 0)
        *ptr++ = val;
    return dest;
}

void *uint32_memset(void *dest, uint32_t val, size_t length) {
    uint32_t *ptr = (uint32_t *)dest;
    while (length-- > 0)
        *ptr++ = val;
    return dest;
}

void *memmove(void *s1, const void *s2, size_t count) {
    bcopy(s2, s1, count);
    return s1;
}

int memcmp(const void *str1, const void *str2, size_t count) {
    const unsigned char *s1 = (const unsigned char*)str1;
    const unsigned char *s2 = (const unsigned char*)str2;

    while (count-- > 0) {
        if (*s1++ != *s2++)
            return s1[-1] < s2[-1] ? -1 : 1;
    }

    return 0;
}