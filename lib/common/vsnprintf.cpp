#include "include/vsnprintf.h"

/*
Code is mostly merged from https://github.com/mpaland/printf which adheres to the MIT License.
The full license text may be found at LICENSES/MIT.txt
*/

// Writes the character into a buffer
static void _out_buffer(char character, void *buffer, size_t idx, size_t maxlength) {
    if (idx < maxlength) ((char *)buffer)[idx] = character;
}

// Sends the character to the user function
static void _out_fct(char character, void *buffer, size_t idx, size_t maxlength) {
    (void)idx; (void)maxlength;
    if (character) {
        const out_fct_wrap_type *wrap = (const out_fct_wrap_type *)buffer;
        wrap->fct(character, wrap->arg);
    }
}

size_t _out_rev(out_fct_type out, void *buffer, size_t idx, size_t maxlength, const char *buf, size_t len, uint32_t width, uint32_t flags) {
    const size_t start_idx = idx;

    if (!(flags & FLAGS_PADLEFT)) {
        const char pad = (flags & FLAGS_ZEROPAD) ? '0' : ' ';
        for (size_t i = len; i < width; i++) out(pad, buffer, idx++, maxlength);
    }

    while (len) out(buf[--len], buffer, idx++, maxlength);
    
    if (flags & FLAGS_PADLEFT) {
        while (idx - start_idx < width) out(' ', buffer, idx++, maxlength);
    }

    return idx;
}

size_t _ntoa_format(out_fct_type out, void *buffer, size_t idx, size_t maxlength, char *buf, size_t len, bool negative, uint32_t base, uint32_t prec, uint32_t width, uint32_t flags) {
    if (!(flags & FLAGS_PADLEFT)) {
        if (width && (flags & FLAGS_ZEROPAD) && (negative || (flags & (FLAGS_FORCEPLUS | FLAGS_PADSPACE)))) width--;
        while ((len < prec) && (len < PRINTF_NTOA_BUFFER_SIZE)) buf[len++] = '0';
        while ((flags & FLAGS_ZEROPAD) && (len < width) && (len < PRINTF_NTOA_BUFFER_SIZE)) buf[len++] = '0';
    }

    if (flags & FLAGS_ZEROHASHPREFIX) {
        if (!(flags & FLAGS_PRECISION) && len && ((len == prec) || (len == width))) {
            len--;
            if (len && (base == 16)) len--;
        }
        if ((base == 16) && !(flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) buf[len++] = 'x';
        else if ((base == 16) && (flags & FLAGS_UPPERCASE) && (len < PRINTF_NTOA_BUFFER_SIZE)) buf[len++] = 'X';
        else if ((base == 2) && (len < PRINTF_NTOA_BUFFER_SIZE)) buf[len++] = 'b';
        if (len < PRINTF_NTOA_BUFFER_SIZE) buf[len++] = '0';
    }

    if (len < PRINTF_NTOA_BUFFER_SIZE) {
        if (negative) buf[len++] = '-';
        else if (flags & FLAGS_FORCEPLUS) buf[len++] = '+';
        else if (flags & FLAGS_PADSPACE) buf[len++] = ' ';
    }

    return _out_rev(out, buffer, idx, maxlength, buf, len, width, flags);
}

size_t _ntoa_long_long(out_fct_type out, void *buffer, size_t idx, size_t maxlength, unsigned long long value, bool negative, unsigned long long base, uint32_t prec, uint32_t width, uint32_t flags) {
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0;

    if (!value) flags &= ~FLAGS_ZEROHASHPREFIX;
    
    if (!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit  - 10;
            value /= base;
        } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return _ntoa_format(out, buffer, idx, maxlength, buf, len, negative, (uint32_t)base, prec, width, flags);
}

size_t _ntoa_long(out_fct_type out, void *buffer, size_t idx, size_t maxlength, unsigned long value, bool negative, unsigned long base, uint32_t prec, uint32_t width, uint32_t flags) {
    char buf[PRINTF_NTOA_BUFFER_SIZE];
    size_t len = 0;

    if (!value) flags &= ~FLAGS_ZEROHASHPREFIX;
    
    if (!(flags & FLAGS_PRECISION) || value) {
        do {
            const char digit = (char)(value % base);
            buf[len++] = digit < 10 ? '0' + digit : (flags & FLAGS_UPPERCASE ? 'A' : 'a') + digit  - 10;
            value /= base;
        } while (value && (len < PRINTF_NTOA_BUFFER_SIZE));
    }

    return _ntoa_format(out, buffer, idx, maxlength, buf, len, negative, (uint32_t)base, prec, width, flags);
}

// Define COMMON_NO_FLOAT to remove %f. The kernel has no floating point support.
#ifndef COMMON_NO_FLOAT
size_t _ftoa(out_fct_type out, void *buffer, size_t idx, size_t maxlength, double value, uint32_t prec, uint32_t width, uint32_t flags) {
    char buf[PRINTF_FTOA_BUFFER_SIZE];
    size_t len = 0;
    double diff = 0.0;

    static const double pow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };

    if (value != value) return _out_rev(out, buffer, idx, maxlength, "nan", 3, width, flags & ~FLAGS_ZEROPAD);
    if (value < -DBL_MAX) return _out_rev(out, buffer, idx, maxlength, "fni-", 4, width, flags & ~FLAGS_ZEROPAD);
    if (value > DBL_MAX) return _out_rev(out, buffer, idx, maxlength, (flags & FLAGS_FORCEPLUS) ? "fni+" : "fni", (flags & FLAGS_FORCEPLUS) ? 4 : 3, width, flags & ~FLAGS_ZEROPAD);

    if ((value > PRINTF_MAX_FLOAT) || (value < -PRINTF_MAX_FLOAT)) return 0; // TODO: should be _etoa()... to implement

    bool negative = false;
    if (value < 0) {
        negative = true;
        value = 0 - value;
    }

    if (!(flags & FLAGS_PRECISION)) prec = PRINTF_DEFAULT_FLOAT_PRECISION;
    while ((len < PRINTF_FTOA_BUFFER_SIZE) && (prec > 9)) {
        buf[len++] = '0';
        prec--;
    }

    int whole = (int)value;
    double tmp = (value - whole) * pow10[prec];
    uint64_t frac = (uint64_t)tmp;
    diff = tmp - frac;

    if (diff > 0.5) {
        ++frac;
        if (frac >= pow10[prec]) {
            frac = 0;
            ++whole;
        }
    } else if (diff < 0.5) {}
    else if ((frac == 0) || (frac & 1)) ++frac;

    if (prec == 0) {
        diff = value - (double) whole;
        if ((!(diff < 0.5) || (diff >0.5)) && (whole & 1)) ++whole;
    } else {
        uint32_t count = prec;

        while (len < PRINTF_FTOA_BUFFER_SIZE) {
            --count;
            buf[len++] = (char)(48 + (frac % 10));
            if (!(frac /= 10)) break;
        }
        while ((len < PRINTF_FTOA_BUFFER_SIZE) && (count-- > 0)) buf[len++] = '0';
        if (len < PRINTF_FTOA_BUFFER_SIZE) buf[len++] = '.';
    }

    while (len < PRINTF_FTOA_BUFFER_SIZE) {
        buf[len++] = (char)(48 + (whole % 10));
        if (!(whole /= 10)) break;
    }

    if (!(flags & FLAGS_PADLEFT) && (flags & FLAGS_ZEROPAD)) {
        if (width && (negative || (flags & (FLAGS_FORCEPLUS | FLAGS_PADSPACE)))) width--;
        while ((len < width) && (len < PRINTF_FTOA_BUFFER_SIZE)) buf[len++] = '0';
    }

    if (len < PRINTF_FTOA_BUFFER_SIZE) {
        if (negative) buf[len++] = '-';
        else if (flags & FLAGS_FORCEPLUS) buf[len++] = '+';
        else if (flags & FLAGS_PADSPACE) buf[len++] = ' ';
    }

    return _out_rev(out, buffer, idx, maxlength, buf, len, width, flags);
}
#endif // COMMON_NO_FLOAT

static inline uint32_t _strnlen_s(const char* str, size_t maxsize) {
  const char* s;
  for (s = str; *s && maxsize--; ++s);
  return (uint32_t)(s - str);
}

static int _format(out_fct_type out, void *buffer, size_t maxlength, const char* format, va_list args) {
    uint32_t flags, precision, width, n;
    size_t idx = 0;

    while (*format) {
        if (*format != '%') { // if it's not a flag, add the char to the out buffer and go next
            out(*format, buffer, idx++, maxlength);
            format++;
            continue;
        } else format++; // otherwise eval it

        flags = 0;
        do {
            switch (*format) {
                case '0': flags |= FLAGS_ZEROPAD;        format++; n = 1; break;
                case '-': flags |= FLAGS_PADLEFT;        format++; n = 1; break;
                case '+': flags |= FLAGS_FORCEPLUS;      format++; n = 1; break;
                case ' ': flags |= FLAGS_PADSPACE;       format++; n = 1; break;
                case '#': flags |= FLAGS_ZEROHASHPREFIX; format++; n = 1; break;
                default:                                           n = 0; break;
            }
        } while (n);

        width = 0U;
        if (is_digit(*format)) width = atoi(&format);
        else if (*format == '*') {
            const int w = __builtin_va_arg(args, int);
            if (w < 0) {
                flags |= FLAGS_PADLEFT;
                width = (uint32_t)-w;
            } else width = (uint32_t)w;
            format++;
        }

        precision = 0U;
        if (*format == '.') {
            flags |= FLAGS_PRECISION;
            format++;
            if (is_digit(*format)) precision = atoi(&format);
            else if (*format == '*') {
                const int prec = (int)__builtin_va_arg(args, int);
                precision = prec > 0 ? (uint32_t)prec : 0U;
                format++;
            }
        }

        switch (*format) {
            case 'l':
                flags |= FLAGS_LONG;
                format++;
                if (*format == 'l') {
                    flags |= FLAGS_LONG_LONG;
                    format++;
                }
                break;
            case 'h':
                flags |= FLAGS_SHORT;
                format++;
                if (*format == 'h') {
                    flags |= FLAGS_CHAR;
                    format++;
                }
                break;
            case 'j':
                flags |= (sizeof(intmax_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            case 'z':
                flags |= (sizeof(size_t) == sizeof(long) ? FLAGS_LONG : FLAGS_LONG_LONG);
                format++;
                break;
            default: break;
        }

        // eval specifier
        switch (*format) {
            case 'd':
            case 'i':
            case 'u':
            case 'x':
            case 'X':
            case 'o':
            case 'b': {
                uint32_t base;
                if (*format == 'x' || *format == 'X') base = 16U;
                else if (*format == 'o') base = 8U;
                else if (*format == 'b') base = 2U;
                else {
                    base = 10U;
                    flags &= ~FLAGS_ZEROHASHPREFIX;
                }

                if (*format == 'X') flags |= FLAGS_UPPERCASE;
                if ((*format != 'i') && (*format != 'd')) flags &= ~(FLAGS_FORCEPLUS | FLAGS_PADSPACE);
                if (flags & FLAGS_PRECISION) flags &= ~FLAGS_ZEROPAD;
                
                // converting the integer
                if ((*format == 'i') || (*format == 'd')) {
                    if (flags & FLAGS_LONG_LONG) {
                        const long long value = __builtin_va_arg(args, long long);
                        idx = _ntoa_long_long(out, buffer, idx, maxlength, (unsigned long long)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                    } else if (flags & FLAGS_LONG) {
                        const long value = __builtin_va_arg(args, long);
                        idx = _ntoa_long(out, buffer, idx, maxlength, (uint64_t)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                    } else {
                        const int value = (flags & FLAGS_CHAR) ? (char)__builtin_va_arg(args, int) : (flags & FLAGS_SHORT) ? (short int)__builtin_va_arg(args, int) : __builtin_va_arg(args, int);
                        idx = _ntoa_long(out, buffer, idx, maxlength, (unsigned int)(value > 0 ? value : 0 - value), value < 0, base, precision, width, flags);
                    }
                } else {
                    if (flags & FLAGS_LONG_LONG) {
                        idx = _ntoa_long_long(out, buffer, idx, maxlength, __builtin_va_arg(args, unsigned long long), false, base, precision, width, flags);
                    } else if (flags & FLAGS_LONG) {
                        idx = _ntoa_long(out, buffer, idx, maxlength, __builtin_va_arg(args, unsigned long), false, base, precision, width, flags);
                    } else {
                        const unsigned int value = (flags & FLAGS_CHAR) ? (unsigned char)__builtin_va_arg(args, unsigned int) : (flags & FLAGS_SHORT) ? (unsigned short int)__builtin_va_arg(args, unsigned int) : __builtin_va_arg(args, unsigned int);
                        idx = _ntoa_long(out, buffer, idx, maxlength, value, false, base, precision, width, flags);
                    }
                }
                
                format++;
                break;
            }

#ifndef COMMON_NO_FLOAT
            case 'f':
            case 'F':
                if (*format == 'F') flags |= FLAGS_UPPERCASE;
                idx = _ftoa(out, buffer, idx, maxlength, __builtin_va_arg(args, double), precision, width, flags);
                format++;
                break;
#endif // COMMON_NO_FLOAT
            // TODO: exponentials... (e, E, g, G)

            case 'c': {
                uint32_t l = 1;
                if (!(flags & FLAGS_PADLEFT)) {
                    while (l++ < width) out(' ', buffer, idx++, maxlength);
                }

                out((char)__builtin_va_arg(args, int), buffer, idx++, maxlength);

                if (flags & FLAGS_PADLEFT) {
                    while (l++ < width) out(' ', buffer, idx++, maxlength);
                }
                format++;
                break;
            }

            case 's': {
                const char* p = __builtin_va_arg(args, char*);
                uint32_t l = _strnlen_s(p, precision ? precision : (size_t)-1);

                if (flags & FLAGS_PRECISION) l = (l < precision ? 1 : precision);
                if (!(flags & FLAGS_PADLEFT)) {
                    while (l++ < width) out(' ', buffer, idx++, maxlength);
                }

                while ((*p != 0) && (!(flags & FLAGS_PRECISION) || precision--)) out(*(p++), buffer, idx++, maxlength);

                if (flags & FLAGS_PADLEFT) {
                    while (l++ < width) out(' ', buffer, idx++, maxlength);
                }

                format++;
                break;
            }

            case 'p': {
                width = sizeof(void*) * 2;
                flags |= FLAGS_ZEROPAD | FLAGS_UPPERCASE;
                const bool is_ll = sizeof(uintptr_t) == sizeof(long long);
                if (is_ll) idx = _ntoa_long_long(out, buffer, idx, maxlength, (uintptr_t)__builtin_va_arg(args, void*), false, 16, precision, width, flags);
                else idx = _ntoa_long(out, buffer, idx, maxlength, (uint64_t)((uintptr_t)__builtin_va_arg(args, void*)), false, 16, precision, width, flags);

                format++;
                break;
            }

            case '%':
                out('%', buffer, idx++, maxlength);
                format++;
                break;
            
            default:
                out(*format, buffer, idx++, maxlength);
                format++;
                break;
        }
    }

    out((char)0, buffer, idx < maxlength ? idx : maxlength - 1, maxlength);

    return (int)idx;
}

int _vsnprintf(char *buffer, size_t maxlength, const char *format, va_list args) {
    return _format(_out_buffer, buffer, maxlength, format, args);
}

int _snprintf(char *buffer, size_t maxlength, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = _format(_out_buffer, buffer, maxlength, format, args);
    va_end(args);
    return ret;
}

int _vfctprintf(void (*fct)(char character, void *arg), void *arg, const char *format, va_list args) {
    const out_fct_wrap_type wrap = { fct, arg };
    return _format(_out_fct, (void *)&wrap, (size_t)-1, format, args);
}

int _fctprintf(void (*fct)(char character, void *arg), void *arg, const char *format, ...) {
    va_list args;
    va_start(args, format);
    const int ret = _vfctprintf(fct, arg, format, args);
    va_end(args);
    return ret;
}
