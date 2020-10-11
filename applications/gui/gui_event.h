#pragma once

#include <stdint.h>
#include <input/input.h>

typedef enum {
    GUI_MESSAGE_TYPE_REDRAW =0x00,
    GUI_MESSAGE_TYPE_INPUT  =0x01
} gui_message_type_t;

typedef struct {
    gui_message_type_t  type;
    InputEvent          input;
    void                *data;
} gui_message_t;

typedef struct gui_event_t gui_event_t;

gui_event_t * gui_event_alloc();

void gui_event_free(gui_event_t *gui_event);

void gui_event_messsage_send(gui_event_t *gui_event, gui_message_t *message);

gui_message_t gui_event_message_next(gui_event_t *gui_event);
