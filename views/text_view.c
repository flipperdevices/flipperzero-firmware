#include <gui/gui.h>
#include "text_view.h"

#define animation_speed 2

static void animation(Canvas* canvas, Fnaf* fnaf) {
    if (fnaf->counter > animation_speed * 10) {
        fnaf->counter = 0;
        fnaf->progress += 1;
        fnaf->current_view = night_number;
        if (fnaf->progress > 6) fnaf->current_view = main_menu;
    } else {
        canvas_set_color(canvas, 1);
        uint8_t y = 35 - fnaf->counter / animation_speed;
        canvas_draw_str(canvas, 50, y, "5");
        canvas_draw_str(canvas, 50, y + 10, "6");
        canvas_set_color(canvas, 0);
        canvas_draw_box(canvas, 50, 37, 5, 9);
        canvas_draw_box(canvas, 50, 16, 5, 9);
        canvas_set_color(canvas, 1);

        fnaf->counter += 1;
    }
}

void text_view(Canvas* canvas, Fnaf* fnaf) {
    switch (fnaf->current_view) {
    case night_number:
        if (fnaf->counter > 25) {
            fnaf->counter = 0;
            fnaf->current_view = office;
        } else {
            canvas_set_bitmap_mode(canvas, 1);
            canvas_set_color(canvas, 1);
            char messages[7][10] = { "1st Night", "2nd Night", "3rd Night", "4th Night", "5th Night", "6th Night", "7th Night" };
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(canvas, 39, 28, "12:00 AM");
            canvas_draw_str(canvas, 41, 40, messages[fnaf->progress]);
        }
        fnaf->counter += 1;
        break;
    case night_complete:
        canvas_set_bitmap_mode(canvas, 1);
        canvas_set_color(canvas, 1);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 58, 35, "AM");
        animation(canvas, fnaf);
        // canvas_draw_str(canvas, 50, 35, "6");
        break;
    case game_over:
        canvas_set_bitmap_mode(canvas, 1);
        canvas_set_color(canvas, 1);
        canvas_draw_str(canvas, 37, 34, "Game Over");
        break;
    }
}
