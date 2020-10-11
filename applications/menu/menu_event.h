#pragma once

#include <stdint.h>
#include <input/input.h>

typedef enum {
    MENU_MESSAGE_TYPE_UP    = 0x00,
    MENU_MESSAGE_TYPE_DOWN  = 0x01,
    MENU_MESSAGE_TYPE_LEFT  = 0x02,
    MENU_MESSAGE_TYPE_RIGHT = 0x03,
    MENU_MESSAGE_TYPE_OK    = 0x04,
    MENU_MESSAGE_TYPE_BACK  = 0x05,
    MENU_MESSAGE_TYPE_IDLE  = 0x06,
    MENU_MESSAGE_TYPE_WTF   = 0xFF
} menu_message_type_t;

typedef struct {
    menu_message_type_t type;
    void                *data;
} menu_message_t;

typedef struct menu_event_t menu_event_t;

menu_event_t * menu_event_alloc();

void menu_event_free(menu_event_t *menu_event);

void menu_event_activity_notify(menu_event_t *menu_event);

menu_message_t menu_event_next(menu_event_t *menu_event);

void menu_event_input_callback(InputEvent *input_event, menu_event_t *menu_event);
