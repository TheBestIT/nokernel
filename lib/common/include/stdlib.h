#ifndef COMMON_STDLIB_H
#define COMMON_STDLIB_H

#include <stdint.h>
#include <stddef.h>

#include "ctype.h"
#include "math.h"

size_t strlen(const char *string);
uint32_t atoi(const char **string);

template<typename T> bool contains(T item, const T array[], size_t size) {
    for (size_t i = 0; i < size; i++) if (array[i] == item) return true;

    return false;
}

#endif // COMMON_STDLIB_H