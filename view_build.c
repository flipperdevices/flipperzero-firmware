/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

extern ProtoViewDecoder *Decoders[];    // Defined in signal.c.

/* Our view private data. */
typedef struct {
    ProtoViewDecoder *decoder;      // Decoder we are using to create a message.
    uint32_t cur_decoder;           // Decoder index when we are yet selecting
                                    // a decoder. Used when decoder is NULL.
    ProtoViewFieldSet *fieldset;    // The fields to populate.
    uint32_t cur_field;             // Field we are editing right now. This
                                    // is the index inside the 'fieldset'
                                    // fields.
} BuildViewPrivData;

/* Render the view to select the decoder, among the ones that
 * support message building. */
static void render_view_select_decoder(Canvas *const canvas, ProtoViewApp *app) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 9, "Signal builder");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 19, "up/down: select, ok: choose");

    UNUSED(app); // XXX
}

/* Render the view that allows the user to populate the fields needed
 * for the selected decoder to build a message. */
static void render_view_set_fields(Canvas *const canvas, ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    char buf[32];
    snprintf(buf,sizeof(buf), "%s field %d/%d",
        privdata->decoder->name, (int)privdata->cur_field,
        (int)privdata->fieldset->numfields);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 9, buf);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 19, "up/down: next field, ok: edit");
}

/* Render the build message view. */
void render_view_build_message(Canvas *const canvas, ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;

    if (privdata->decoder == NULL)
        render_view_select_decoder(canvas,app);
    else
        render_view_set_fields(canvas,app);
}

/* Handle input for the build message view. */
void process_input_build_message(ProtoViewApp *app, InputEvent input) {
    UNUSED(app);
    if (input.type == InputTypeShort) {
        if (input.key == InputKeyOk) {
        } else if (input.key == InputKeyDown) {
        } else if (input.key == InputKeyUp) {
        }
    }
}

/* Called on exit for cleanup. */
void view_exit_build_message(ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    if (privdata->fieldset) fieldset_free(privdata->fieldset);
}
