/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#include "app.h"

extern ProtoViewDecoder *Decoders[];    // Defined in signal.c.

/* Our view private data. */
#define USER_VALUE_LEN 64
typedef struct {
    ProtoViewDecoder *decoder;      /* Decoder we are using to create a
                                       message. */
    uint32_t cur_decoder;           /* Decoder index when we are yet selecting
                                       a decoder. Used when decoder is NULL. */
    ProtoViewFieldSet *fieldset;    /* The fields to populate. */
    uint32_t cur_field;             /* Field we are editing right now. This
                                       is the index inside the 'fieldset'
                                       fields. */
    char *user_value;               /* Keyboard input to replace the current
                                       field value goes here. */
} BuildViewPrivData;

/* Not all the decoders support message bulding, so we can't just
 * increment / decrement the cur_decoder index here. */
static void select_next_decoder(ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    do { 
        privdata->cur_decoder++;
        if (Decoders[privdata->cur_decoder] == NULL)
            privdata->cur_decoder = 0;
    } while(Decoders[privdata->cur_decoder]->get_fields == NULL);
}

/* Like select_next_decoder() but goes backward. */
static void select_prev_decoder(ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    do {
        if (privdata->cur_decoder == 0) {
            /* Go one after the last one to wrap around. */
            while(Decoders[privdata->cur_decoder]) privdata->cur_decoder++;
        }
        privdata->cur_decoder--;
    } while(Decoders[privdata->cur_decoder]->get_fields == NULL);
}

/* Render the view to select the decoder, among the ones that
 * support message building. */
static void render_view_select_decoder(Canvas *const canvas, ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 9, "Signal creator");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 0, 19, "up/down: select, ok: choose");

    // When entering the view, the current decoder is just set to zero.
    // Seek the next valid if needed.
    if (Decoders[privdata->cur_decoder]->get_fields == NULL)
        select_next_decoder(app);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas,64,38,AlignCenter,AlignCenter,
        Decoders[privdata->cur_decoder]->name);
}

/* Render the view that allows the user to populate the fields needed
 * for the selected decoder to build a message. */
static void render_view_set_fields(Canvas *const canvas, ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    char buf[32];
    snprintf(buf,sizeof(buf), "%s field %d/%d",
        privdata->decoder->name, (int)privdata->cur_field+1,
        (int)privdata->fieldset->numfields);
    canvas_set_color(canvas,ColorBlack);
    canvas_draw_box(canvas,0,0,128,21);
    canvas_set_color(canvas,ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 1, 9, buf);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 1, 19, "up/down: next field, ok: edit");

    /* Write the field name, type, current content. */
    canvas_set_color(canvas,ColorBlack);
    ProtoViewField *field = privdata->fieldset->fields[privdata->cur_field];
    snprintf(buf,sizeof(buf), "%s %s:%d", field->name,
        field_get_type_name(field), (int)field->len);
    buf[0] = toupper(buf[0]);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas,64,30,AlignCenter,AlignCenter,buf);
    canvas_set_font(canvas, FontSecondary);

    /* Render the current value between "" */
    unsigned int written = (unsigned int) field_to_string(buf+1,sizeof(buf)-1,field);
    buf[0] = '"';
    if (written+3 < sizeof(buf)) memcpy(buf+written+1,"\"\x00",2);
    canvas_draw_str_aligned(canvas,63,45,AlignCenter,AlignCenter,buf);

    /* Footer instructions. */
    canvas_draw_str(canvas, 0, 62, "Long ok: create, < > incr/decr");
}

/* Render the build message view. */
void render_view_build_message(Canvas *const canvas, ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;

    if (privdata->decoder)
        render_view_set_fields(canvas,app);
    else
        render_view_select_decoder(canvas,app);
}

/* Handle input for the decoder selection. */
static void process_input_select_decoder(ProtoViewApp *app, InputEvent input) {
    BuildViewPrivData *privdata = app->view_privdata;
    if (input.type == InputTypeShort) {
        if (input.key == InputKeyOk) {
            privdata->decoder = Decoders[privdata->cur_decoder];
            privdata->fieldset = fieldset_new();
            privdata->decoder->get_fields(privdata->fieldset);
            // Now we use the subview system in order to protect the
            // message editing mode from accidental < or > presses.
            // Since we are technically into a subview now, we'll have
            // control of < and >.
            InputEvent ii = {.type = InputTypePress, .key = InputKeyDown};
            ui_process_subview_updown(app,ii,2);
        } else if (input.key == InputKeyDown) {
            select_next_decoder(app);
        } else if (input.key == InputKeyUp) {
            select_prev_decoder(app);
        }
    }
}

/* Called after the user typed the new field value in the keyboard.
 * Let's save it and remove the keyboard view. */
static void text_input_done_callback(void* context) {
    ProtoViewApp *app = context;
    BuildViewPrivData *privdata = app->view_privdata;

    if (field_set_from_string(privdata->fieldset->fields[privdata->cur_field],
            privdata->user_value, strlen(privdata->user_value)) == false)
    {
        ui_show_alert(app, "Invalid value", 1500);
    }

    free(privdata->user_value);
    privdata->user_value = NULL;
    ui_dismiss_keyboard(app);
}

/* Handle input for fields editing mode. */
static void process_input_set_fields(ProtoViewApp *app, InputEvent input) {
    BuildViewPrivData *privdata = app->view_privdata;
    ProtoViewFieldSet *fs = privdata->fieldset;
    if (input.type == InputTypeShort) {
        if (input.key == InputKeyOk) {
            /* Show the keyboard to let the user type the new
             * value. */
            if (privdata->user_value == NULL)
                privdata->user_value = malloc(USER_VALUE_LEN);
            field_to_string(privdata->user_value, USER_VALUE_LEN,
                            fs->fields[privdata->cur_field]);
            ui_show_keyboard(app, privdata->user_value, USER_VALUE_LEN,
                             text_input_done_callback);
        } else if (input.key == InputKeyDown) {
            privdata->cur_field = (privdata->cur_field+1) % fs->numfields;
        } else if (input.key == InputKeyUp) {
            if (privdata->cur_field == 0)
                privdata->cur_field = fs->numfields-1;
            else
                privdata->cur_field--;
        }
    }
}

/* Handle input for the build message view. */
void process_input_build_message(ProtoViewApp *app, InputEvent input) {
    BuildViewPrivData *privdata = app->view_privdata;
    if (privdata->decoder)
        process_input_set_fields(app,input);
    else
        process_input_select_decoder(app,input);
}

/* Called on exit for cleanup. */
void view_exit_build_message(ProtoViewApp *app) {
    BuildViewPrivData *privdata = app->view_privdata;
    if (privdata->fieldset) fieldset_free(privdata->fieldset);
    if (privdata->user_value) free(privdata->user_value);
}
