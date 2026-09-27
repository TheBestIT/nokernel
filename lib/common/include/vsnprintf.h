#ifndef COMMON_VSNPRINTF_H
#define COMMON_VSNPRINTF_H

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>

#include "ctype.h"
#include "stdlib.h"

#define PRINTF_NTOA_BUFFER_SIZE 32
#define PRINTF_FTOA_BUFFER_SIZE 32
#define PRINTF_MAX_FLOAT 1e9
#define PRINTF_DEFAULT_FLOAT_PRECISION 6

#define DBL_MAX 1.7976931348623158e+308

// FLAGS

#define FLAGS_ZEROPAD           (1 << 0) // Pads with 0:            eg. [%02d] -> [0024] 
#define FLAGS_PADLEFT           (1 << 1) // Pads left:              eg. [%-2d] -> [2400]
#define FLAGS_FORCEPLUS         (1 << 2) // Forces the sign:        eg. [%+d] -> [+24]
#define FLAGS_PADSPACE          (1 << 3) // Forces the space:       eg. [% d] -> [ 24]
#define FLAGS_ZEROHASHPREFIX    (1 << 4) // Adds the hex prefix:    eg. [$#x] -> [0x18]
#define FLAGS_PRECISION         (1 << 5)
#define FLAGS_LONG              (1 << 6)
#define FLAGS_LONG_LONG         (1 << 7)
#define FLAGS_SHORT             (1 << 8)
#define FLAGS_CHAR              (1 << 9)
#define FLAGS_UPPERCASE         (1 << 10)

// -----

typedef void (*out_fct_type)(char character, void *buffer, size_t idx, size_t maxlength);

typedef struct {
    void (*fct)(char character, void *arg);
    void *arg;
} out_fct_wrap_type;

int _snprintf(char *buffer, size_t maxlength, const char *format, ...);
int _vsnprintf(char *buffer, size_t maxlength, const char *format, va_list args);

int _fctprintf(void (*fct)(char character, void *arg), void *arg, const char *format, ...);
int _vfctprintf(void (*fct)(char character, void *arg), void *arg, const char *format, va_list args);

#endif // COMMON_VSNPRINTF_H