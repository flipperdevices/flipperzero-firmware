#pragma once

#include <stdint.h>
#include <u8g2.h>

#define COLOR_WHITE 0x00
#define COLOR_BLACK 0x01

#define CANVAS_FONT_PRIMARY   u8g2_font_Born2bSportyV2_tr
#define CANVAS_FONT_SECONDARY u8g2_font_HelvetiPixel_tr


typedef struct canvas_t canvas_t;
typedef const uint8_t * font_t;

canvas_t * canvas_alloc();

void canvas_free(canvas_t *canvas);

void canvas_commit(canvas_t *canvas);

uint8_t canvas_width(canvas_t *canvas);
uint8_t canvas_height(canvas_t *canvas);

void canvas_clear(canvas_t *canvas);

void canvas_color_set(canvas_t *canvas, uint8_t color);

void canvas_font_set(canvas_t *canvas, font_t font);

void canvas_str_draw(canvas_t *canvas, uint8_t x, uint8_t y, const char *str);

