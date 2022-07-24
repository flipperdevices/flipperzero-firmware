#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <furi/core/check.h>
#include <furi/core/common_defines.h>

int __wrap_printf(const char* format, ...) {
    UNUSED(format);
    return 0;
}

int __wrap_vsnprintf(char* str, size_t size, const char* format, va_list args) {
    UNUSED(str);
    UNUSED(size);
    UNUSED(format);
    UNUSED(args);

    return 0;
}

int __wrap_puts(const char* str) {
    UNUSED(str);
    return 0;
}

int __wrap_putchar(int ch) {
    UNUSED(ch);
    return 0;
}

int __wrap_putc(int ch, FILE* stream) {
    UNUSED(ch);
    UNUSED(stream);
    return 0;
}

int __wrap_snprintf(char* str, size_t size, const char* format, ...) {
    va_list args;
    va_start(args, format);
    int ret = __wrap_vsnprintf(str, size, format, args);
    va_end(args);

    return ret;
}

__attribute__((__noreturn__)) void __wrap___assert(const char* file, int line, const char* e) {
    UNUSED(file);
    UNUSED(line);
    furi_crash(e);
}

__attribute__((__noreturn__)) void
    __wrap___assert_func(const char* file, int line, const char* func, const char* e) {
    UNUSED(file);
    UNUSED(line);
    UNUSED(func);
    furi_crash(e);
}