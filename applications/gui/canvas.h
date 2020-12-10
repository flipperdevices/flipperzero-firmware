#pragma once

#include <stdint.h>
#include <u8g2.h>
#include <gui/icon.h>

typedef enum {
    ColorWhite = 0x00,
    ColorBlack = 0x01,
} Color;

typedef enum { FontPrimary = 0x00, FontSecondary = 0x01, FontGlyph = 0x02 } Font;

typedef struct Canvas Canvas;

uint8_t canvas_width(Canvas* canvas);

uint8_t canvas_height(Canvas* canvas);

void canvas_clear(Canvas* canvas);

void canvas_set_color(Canvas* canvas, Color color);

void canvas_set_font(Canvas* canvas, Font font);

void canvas_draw_str(Canvas* canvas, uint8_t x, uint8_t y, const char* str);

void canvas_draw_icon(Canvas* canvas, uint8_t x, uint8_t y, Icon* icon);

void canvas_draw_xbm(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    uint8_t w,
    uint8_t h,
    const uint8_t* bitmap);

void canvas_draw_dot(Canvas* canvas, uint8_t x, uint8_t y);

void canvas_draw_box(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void canvas_draw_frame(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height);

void canvas_draw_line(Canvas* canvas, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

void canvas_draw_glyph(Canvas* canvas, uint8_t x, uint8_t y, uint16_t ch);
