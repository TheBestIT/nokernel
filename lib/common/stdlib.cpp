#include "include/stdlib.h"

size_t strlen(const char* string) {
    size_t count = 0;
    while (*string) {
        count++;
        string++;
    }
    return count;
}

uint32_t atoi(const char **string) {
    uint32_t sum = 0;

    while (is_digit(**string)) {
        uint32_t digit = ((uint32_t)(**string) - (uint32_t)'0');
        sum = sum * 10 + digit;
        (*string)++;
    }

    return sum;
}