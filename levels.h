#include <stdint.h>
#include "space_impact_app_icons.h"

typedef struct {
    uint8_t index;
    bool isInvertedColor;
} Level;

static Level levels[2] = {
    {0, true},
    {1, false},
};

static void level_draw_bg(Canvas* const canvas, uint8_t index, bool isInverted) {
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