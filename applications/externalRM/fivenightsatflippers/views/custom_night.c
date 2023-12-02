#include "custom_night.h"
#include "../fnaf.h"

void draw_custom_night_menu(Canvas* canvas, void* ctx) {
    Fnaf* fnaf = ctx;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 22, 11, "Customize Night");
    // names
    canvas_draw_str(canvas, 30, 24, "Flipper");
    canvas_draw_str(canvas, 30, 35, "Blipper");
    canvas_draw_str(canvas, 28, 46, "Chipper");
    canvas_draw_str(canvas, 30, 57, "Fopper");
    // levels
    char levels[4][4];
    for(uint8_t i = 0; i < 4; i++) {
        snprintf(levels[i], 4, "%u", fnaf->dolphins->AI[i]);
    }
    canvas_draw_str_aligned(canvas, 92, 25, AlignRight, AlignBottom, levels[Flipper]);
    canvas_draw_str_aligned(canvas, 92, 36, AlignRight, AlignBottom, levels[Blipper]);
    canvas_draw_str_aligned(canvas, 92, 47, AlignRight, AlignBottom, levels[Chipper]);
    canvas_draw_str_aligned(canvas, 92, 58, AlignRight, AlignBottom, levels[Fopper]);
    // arrows
    canvas_draw_line(
        canvas, 78, 19 + fnaf->custom_menu_cursor * 11, 76, 21 + fnaf->custom_menu_cursor * 11);
    canvas_draw_line(
        canvas, 76, 21 + fnaf->custom_menu_cursor * 11, 78, 23 + fnaf->custom_menu_cursor * 11);
    canvas_draw_line(
        canvas, 94, 19 + fnaf->custom_menu_cursor * 11, 96, 21 + fnaf->custom_menu_cursor * 11);
    canvas_draw_line(
        canvas, 96, 21 + fnaf->custom_menu_cursor * 11, 94, 23 + fnaf->custom_menu_cursor * 11);
}

void custom_night_input(void* ctx) {
    Fnaf* fnaf = ctx;
    if(fnaf->event.type == InputTypePress) {
        switch(fnaf->event.key) {
        case InputKeyLeft:
            if(fnaf->dolphins->AI[fnaf->custom_menu_cursor] < 20)
                fnaf->dolphins->AI[fnaf->custom_menu_cursor] -= 1;
            break;
        case InputKeyRight:
            if(fnaf->dolphins->AI[fnaf->custom_menu_cursor] < 20)
                fnaf->dolphins->AI[fnaf->custom_menu_cursor] += 1;
            break;
        case InputKeyUp:
            if(fnaf->custom_menu_cursor > 0) fnaf->custom_menu_cursor -= 1;
            break;
        case InputKeyDown:
            if(fnaf->custom_menu_cursor < 3) fnaf->custom_menu_cursor += 1;
            break;
        case InputKeyOk:
            night_start(fnaf);
            break;
        case InputKeyBack:
            fnaf->custom_night = false;
            SWITCH_VIEW(main_menu);
            break;
        default:
            break;
        }
    }
    if(fnaf->dolphins->AI[fnaf->custom_menu_cursor] > 20)
        fnaf->dolphins->AI[fnaf->custom_menu_cursor] = 20;
}
