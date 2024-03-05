#pragma once
#include <stddef.h>
#include <gui/canvas.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Print formatted string to canvas
 * 
 * @param canvas canvas instance
 * @param x x position
 * @param y y position
 * @param format format string
 * @param ...  arguments
 */
void canvas_printf(Canvas* canvas, uint8_t x, uint8_t y, const char* format, ...)
    __attribute__((__format__(__printf__, 4, 5)));

/**
 * @brief Get width of formatted string
 * 
 * @param canvas canvas instance
 * @param format format string
 * @param ... arguments
 * @return size_t width of formatted string
 */
size_t canvas_printf_width(Canvas* canvas, const char* format, ...)
    __attribute__((__format__(__printf__, 2, 3)));

/**
 * @brief Print formatted string to canvas with alignment
 * 
 * @param canvas canvas instance
 * @param x x position
 * @param y y position
 * @param h horizontal alignment
 * @param v vertical alignment
 * @param format format string
 * @param ... arguments
 */
void canvas_printf_aligned(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    Align h,
    Align v,
    const char* format,
    ...) __attribute__((__format__(__printf__, 6, 7)));

/**
 * @brief Draw aligned string with outline
 * 
 * @param canvas 
 * @param x 
 * @param y 
 * @param h 
 * @param v 
 * @param cstr 
 */
void canvas_draw_str_aligned_outline(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    Align h,
    Align v,
    const char* cstr);

#ifdef __cplusplus
}
#endif
