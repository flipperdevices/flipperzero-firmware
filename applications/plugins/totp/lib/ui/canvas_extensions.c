#include "canvas_extensions.h"

void canvas_draw_dots(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t width, uint8_t height, const uint8_t *dots) {
    for (uint8_t i = 0; i < width; i++) {
        for (uint8_t j = 0; j < height; j++) {
            if (dots[i + j * width]) {
                canvas_draw_dot(canvas, x + i, y + j);
            }
        }
    }
}
