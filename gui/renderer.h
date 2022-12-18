#include <stdint.h>
#include <gui/canvas.h>

bool inverted = false;

static void draw_point(Canvas* const canvas, uint8_t x, uint8_t y) {
    canvas_set_color(canvas, inverted ? ColorWhite : ColorBlack);
    canvas_draw_dot(canvas, x, y);
}

static void draw_ui_asset(Canvas* const canvas, uint8_t x, uint8_t y, uint8_t* asset) {
    uint8_t index = 2;

    for(int n = 0; n < asset[1]; n++) {
        for(int i = 0; i < asset[0]; i++) {
            if(asset[index] == 1) {
                draw_point(canvas, x + i, y + n);
            }
            index++;
        }
    }
}