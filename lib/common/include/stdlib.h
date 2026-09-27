#ifndef COMMON_STDLIB_H
#define COMMON_STDLIB_H

#include <stdint.h>
#include <stddef.h>

#include "ctype.h"
#include "math.h"

size_t strlen(const char *string);
uint32_t atoi(const char **string);

#endif // COMMON_STDLIB_H