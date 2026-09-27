#include <stdio.h>
#include <assert.h>
#include <memory.h>

#include "lib/common/include/stdlib.h"
#include "lib/common/include/math.h"
#include "lib/common/include/vsnprintf.h"

static void _putchar(char character, void *arg) {
    (void)arg;
    putchar(character);
}

int printf(const char* format, ...) {
    va_list va;
    va_start(va, format);
    const int ret = _vfctprintf(_putchar, NULL, format, va);
    va_end(va);
    return ret;
}

int main() {
    char buf1[64], buf2[64];
    snprintf(buf1, 64, "%081x", 0xdeadbeefUL);
    _snprintf(buf2, 64, "%081x", 0xdeadbeefUL);

    assert(strcmp(buf1, buf2) == 0);

    printf("[%08x] [%-5d] [%s] AURA\n", 0xbeefU, 42, "fct output");
    return 0;
}