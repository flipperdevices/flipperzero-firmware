#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <furi/core/common_defines.h>

int __wrap_printf(const char* format, ...) {
    UNUSED(format);
    return 0;
}

int __wrap_fiprintf(FILE* file, const char* format, ...) {
    UNUSED(file);
    UNUSED(format);
    return 0;
}

int __wrap_sprintf(char* str, const char* format, ...) {
    UNUSED(str);
    UNUSED(format);
    return 0;
}

int __wrap_siprintf(char* str, const char* format, ...) {
    UNUSED(str);
    UNUSED(format);
    return 0;
}

int __wrap_snprintf(char* str, size_t size, const char* format, ...) {
    UNUSED(str);
    UNUSED(size);
    UNUSED(format);
    return 0;
}

int __wrap_sniprintf(char* str, size_t size, const char* format, ...) {
    UNUSED(str);
    UNUSED(size);
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