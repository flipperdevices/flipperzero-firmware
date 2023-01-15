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
    if (app->signal_decoded == false) {
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 30,36,"No signal decoded");
        return;
    }

    /* Protocol name as title. */
    canvas_set_font(canvas, FontPrimary);
    uint8_t y = 8, lineheight = 10;
    canvas_draw_str(canvas, 0, y, app->signal_info.name);
    y += lineheight;

    /* Info fields. */
    char buf[128];
    canvas_set_font(canvas, FontSecondary);
    if (app->signal_info.raw[0]) {
        snprintf(buf,sizeof(buf),"Raw: %s", app->signal_info.raw);
        canvas_draw_str(canvas, 0, y, buf);
        y += lineheight;
    }
    canvas_draw_str(canvas, 0, y, app->signal_info.info1); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->signal_info.info2); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->signal_info.info3); y += lineheight;
    canvas_draw_str(canvas, 0, y, app->signal_info.info4); y += lineheight;
}

/* Render view with save option. */
static void render_subview_save(Canvas *const canvas, ProtoViewApp *app) {
    UNUSED(canvas);
    UNUSED(app);
}

/* Render the selected subview of this view. */
void render_view_info(Canvas *const canvas, ProtoViewApp *app) {
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
