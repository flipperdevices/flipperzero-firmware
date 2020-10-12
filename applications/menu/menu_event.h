#pragma once

#include <stdint.h>
#include <input/input.h>

typedef enum {
    MenuMessageTypeUp       = 0x00,
    MenuMessageTypeDown     = 0x01,
    MenuMessageTypeLeft     = 0x02,
    MenuMessageTypeRight    = 0x03,
    MenuMessageTypeOk       = 0x04,
    MenuMessageTypeBack     = 0x05,
    MenuMessageTypeIdle     = 0x06,
    MenuMessageTypeUnknown  = 0xFF,
} MenuMessageType;

typedef struct {
    MenuMessageType type;
    void                *data;
} menu_message_t;

typedef struct menu_event_t menu_event_t;

menu_event_t * menu_event_alloc();

void menu_event_free(menu_event_t *menu_event);

void menu_event_activity_notify(menu_event_t *menu_event);

menu_message_t menu_event_next(menu_event_t *menu_event);

void menu_event_input_callback(InputEvent *input_event, void *context);
