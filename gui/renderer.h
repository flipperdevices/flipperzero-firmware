#include <stdint.h>
#include <gui/canvas.h>
#include "space_impact_app_icons.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

static bool isInverted;
static bool level[SCREEN_WIDTH][SCREEN_HEIGHT];

static void render_draw(Canvas* const canvas) {
    for(int x = 0; x < SCREEN_WIDTH; x++) {
        for(int y = 0; y < SCREEN_HEIGHT; y++) {
            if(level[x][y]) {
                canvas_set_color(canvas, isInverted ? ColorWhite : ColorBlack);
                canvas_draw_dot(canvas, x, y);
            }
        }
    }
}

static void render_clear(bool inverted) {
    isInverted = inverted;

    for(int x = 0; x < SCREEN_WIDTH; x++) {
        for(int y = 0; y < SCREEN_HEIGHT; y++) {
            level[x][y] = false;
        }
    }
}

static void draw_point(uint8_t x, uint8_t y) {
    if(x > SCREEN_WIDTH || y > SCREEN_HEIGHT) return;

    level[x][y] = true;
}

static void draw_ui_asset(uint8_t x, uint8_t y, uint8_t* const asset) {
    uint8_t index = 2;

    for(int n = 0; n < asset[1]; n++) {
        for(int i = 0; i < asset[0]; i++) {
            if(asset[index] == 1) {
                draw_point(x + i, y + n);
            }
            index++;
        }
    }
}

static void draw_level_bg(Canvas* const canvas, uint8_t index, bool isInverted) {
    canvas_set_color(canvas, isInverted ? ColorWhite : ColorBlack);
    switch(index) {
    case 0:
        canvas_draw_icon(canvas, 0, 46, &I_level_bg_1);
        break;
    case 1:
        canvas_draw_icon(canvas, 0, 46, &I_level_bg_1);
        break;

    /* no background */
    default:
        break;
    }
}

static void draw_destroy(Canvas* const canvas, uint8_t frame, uint8_t x, uint8_t y) {
    if(frame == 1) {
        canvas_draw_icon(canvas, x, y, &I_destroy1);
    } else if(frame == 2) {
        canvas_draw_icon(canvas, x, y, &I_destroy2);
    }
}