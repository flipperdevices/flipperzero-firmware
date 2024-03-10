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

void canvas_printf_aligned(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    Align h,
    Align v,
    const char* format,
    ...) {
    FuriString* string = furi_string_alloc();
    va_list args;
    va_start(args, format);
    furi_string_vprintf(string, format, args);
    va_end(args);

    canvas_draw_str_aligned(canvas, x, y, h, v, furi_string_get_cstr(string));

    furi_string_free(string);
}

void canvas_draw_str_aligned_outline(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    Align h,
    Align v,
    const char* cstr) {
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, x + 1, y + 0, h, v, cstr);
    canvas_draw_str_aligned(canvas, x - 1, y - 0, h, v, cstr);
    canvas_draw_str_aligned(canvas, x + 0, y + 1, h, v, cstr);
    canvas_draw_str_aligned(canvas, x - 0, y - 1, h, v, cstr);
    canvas_invert_color(canvas);
    canvas_draw_str_aligned(canvas, x, y, h, v, cstr);
}