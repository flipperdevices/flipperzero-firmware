/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

enum {
    SubViewInfoMain,
    SubViewInfoSave,
    SubViewInfoLast, /* Just a sentinel. */
};

/* Render the view with the detected message information. */
static void render_subview_main(Canvas *const canvas, ProtoViewApp *app) {
    /* Protocol name as title. */
    canvas_set_font(canvas, FontPrimary);
    uint8_t y = 8, lineheight = 10;
    canvas_draw_str(canvas, 0, y, app->msg_info->name);
    y += lineheight;

    /* Info fields. */
    char buf[128];
    canvas_set_font(canvas, FontSecondary);
    if (app->msg_info->raw[0]) {
        snprintf(buf,sizeof(buf),"Raw: %s", app->msg_info->raw);
        canvas_draw_str(canvas, 0, y, buf);
        y += lineheight;
    }
    canvas_draw_str(canvas, 0, y, app->msg_info->info1); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->msg_info->info2); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->msg_info->info3); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->msg_info->info4); y += lineheight;
}

/* Render view with save option. */
static void render_subview_save(Canvas *const canvas, ProtoViewApp *app) {
    uint8_t rows = 6;
    uint8_t rowheight = 8;
    uint8_t bitwidth = 4;
    uint8_t bitheight = 5;
    uint32_t idx = 0;
    bool prevbit = false;
    for (uint8_t y = bitheight; y < rows*rowheight; y += rowheight) {
        for (uint8_t x = 5; x < 128; x += 4) {
            bool bit = bitmap_get(app->msg_info->bits,
                                  app->msg_info->bits_bytes,idx++);
            uint8_t prevy = y + prevbit*bitheight - 1;
            uint8_t thisy = y + bit*bitheight - 1;
            canvas_draw_line(canvas,x,prevy,x,thisy);
            canvas_draw_line(canvas,x,thisy,x+bitwidth-1,thisy);
        }
    }
}

/* Render the selected subview of this view. */
void render_view_info(Canvas *const canvas, ProtoViewApp *app) {
    if (app->signal_decoded == false) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 30,36,"No signal decoded");
        return;
    }

    show_available_subviews(canvas,app,SubViewInfoLast);
    switch(app->current_subview[app->current_view]) {
    case SubViewInfoMain: render_subview_main(canvas,app); break;
    case SubViewInfoSave: render_subview_save(canvas,app); break;
    }
}

/* Handle input for the info view. */
void process_input_info(ProtoViewApp *app, InputEvent input) {
    if (process_subview_updown(app,input,SubViewInfoLast)) return;
    if (input.type == InputTypeShort) {
        if (input.key == InputKeyOk) {
            /* Reset the current sample to capture the next. */
            reset_current_signal(app);
        }
    }
}
