#include <furi.h>
#include "canvas.h"

void canvas_printf(Canvas* canvas, uint8_t x, uint8_t y, const char* format, ...) {
    FuriString* string = furi_string_alloc();
    va_list args;
    va_start(args, format);
    furi_string_vprintf(string, format, args);
    va_end(args);

    canvas_draw_str(canvas, x, y, furi_string_get_cstr(string));

    furi_string_free(string);
}

size_t canvas_printf_width(Canvas* canvas, const char* format, ...) {
    FuriString* string = furi_string_alloc();
    va_list args;
    va_start(args, format);
    furi_string_vprintf(string, format, args);
    va_end(args);

    size_t size = canvas_string_width(canvas, furi_string_get_cstr(string));

    furi_string_free(string);

    return size;
}