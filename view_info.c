/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"
#include <gui/view_i.h>

enum {
    SubViewInfoMain,
    SubViewInfoSave,
    SubViewInfoLast, /* Just a sentinel. */
};

/* Our view private data. */
#define SAVE_FILENAME_LEN 64
typedef struct {
    /* Our save view displays an oscilloscope-alike resampled signal,
     * so that the user can see what they are saving. With left/right
     * you can move to next rows. Here we store where we are. */
    uint32_t signal_display_start_row;
    char *filename;
} InfoViewPrivData;

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
    InfoViewPrivData *privdata = app->view_privdata;

    /* Display our signal in digital form: here we don't show the
     * signal with the exact timing of the received samples, but as it
     * is in its logic form, in exact multiples of the short pulse length. */
    uint8_t rows = 6;
    uint8_t rowheight = 11;
    uint8_t bitwidth = 4;
    uint8_t bitheight = 5;
    uint32_t idx = privdata->signal_display_start_row * (128/4);
    bool prevbit = false;
    for (uint8_t y = bitheight+12; y <= rows*rowheight; y += rowheight) {
        for (uint8_t x = 0; x < 128; x += 4) {
            bool bit = bitmap_get(app->msg_info->bits,
                                  app->msg_info->bits_bytes,idx);
            uint8_t prevy = y + prevbit*(bitheight*-1) - 1;
            uint8_t thisy = y + bit*(bitheight*-1) - 1;
            canvas_draw_line(canvas,x,prevy,x,thisy);
            canvas_draw_line(canvas,x,thisy,x+bitwidth-1,thisy);
            prevbit = bit;
            if (idx >= app->msg_info->pulses_count) {
                canvas_set_color(canvas, ColorWhite);
                canvas_draw_dot(canvas, x+1,thisy);
                canvas_draw_dot(canvas, x+3,thisy);
                canvas_set_color(canvas, ColorBlack);
            }
            idx++; // Draw next bit
        }
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 6, "ok: save, < >: slide rows");
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

void text_input_done_callback(void* context) {
    ProtoViewApp *app = context;
    InfoViewPrivData *privdata = app->view_privdata;
    free(privdata->filename);
    view_dispatcher_stop(app->view_dispatcher);
}

/* Handle input for the info view. */
void process_input_info(ProtoViewApp *app, InputEvent input) {
    if (process_subview_updown(app,input,SubViewInfoLast)) return;
    InfoViewPrivData *privdata = app->view_privdata;
    int subview = get_current_subview(app);

    /* Main subview. */
    if (subview == SubViewInfoMain) {
        if (input.type == InputTypeShort && input.key == InputKeyOk) {
            /* Reset the current sample to capture the next. */
            reset_current_signal(app);
        }
    } else if (subview == SubViewInfoSave) {
    /* Save subview. */
        if (input.type == InputTypePress && input.key == InputKeyRight) {
            privdata->signal_display_start_row++;
        } else if (input.type == InputTypePress && input.key == InputKeyLeft) {
            if (privdata->signal_display_start_row != 0)
                privdata->signal_display_start_row--;
        } else if (input.type == InputTypePress && input.key == InputKeyOk) {
            privdata->filename = malloc(SAVE_FILENAME_LEN);
            app->show_text_input = true;
            app->text_input_buffer = privdata->filename;
            app->text_input_buffer_len = SAVE_FILENAME_LEN;
            app->text_input_done_callback = text_input_done_callback;
        }
    }
}
