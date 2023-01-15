/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

/* Called by view rendering callback that has subviews, to show small triangles
 * facing down/up if there are other subviews the user can access with up
 * and down. */
void show_available_subviews(Canvas *canvas, ProtoViewApp *app,
                             int last_subview)
{
    int subview = app->current_subview[app->current_view];
    if (subview != 0)
        canvas_draw_triangle(canvas,120,5,8,5,CanvasDirectionBottomToTop);
    if (subview != last_subview-1)
        canvas_draw_triangle(canvas,120,59,8,5,CanvasDirectionTopToBottom);
}

/* Handle up/down keys when we are in a subview. If the function catched
 * such keypress, it returns true, so that the actual view input callback
 * knows it can just return ASAP without doing anything. */
bool process_subview_updown(ProtoViewApp *app, InputEvent input, int last_subview) {
    int subview = app->current_subview[app->current_view];
    if (input.type == InputTypePress) {
        if (input.key == InputKeyUp) {
            if (subview != 0)
                app->current_subview[app->current_view]--;
            return true;
        } else if (input.key == InputKeyDown) {
            if (subview != last_subview-1)
                app->current_subview[app->current_view]++;
            return true;
        }
    }
    return false;
}

void canvas_draw_str_with_border(Canvas* canvas, uint8_t x, uint8_t y, const char* str, Color text_color, Color border_color)
{
    struct {
        uint8_t x; uint8_t y;
    } dir[8] = {
        {-1,-1},
        {0,-1},
        {1,-1},
        {1,0},
        {1,1},
        {0,1},
        {-1,1},
        {-1,0}
    };

    /* Rotate in all the directions writing the same string to create a
     * border, then write the actual string in the other color in the
     * middle. */
    canvas_set_color(canvas, border_color);
    for (int j = 0; j < 8; j++)
        canvas_draw_str(canvas,x+dir[j].x,y+dir[j].y,str);
    canvas_set_color(canvas, text_color);
    canvas_draw_str(canvas,x,y,str);
    canvas_set_color(canvas, ColorBlack);
}
