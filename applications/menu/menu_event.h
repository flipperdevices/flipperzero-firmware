#pragma once

#include <stdint.h>

#define MENU_MESSAGE_TYPE_UP    0x00
#define MENU_MESSAGE_TYPE_DOWN  0x01
#define MENU_MESSAGE_TYPE_LEFT  0x02
#define MENU_MESSAGE_TYPE_RIGHT 0x03
#define MENU_MESSAGE_TYPE_OK    0x04
#define MENU_MESSAGE_TYPE_BACK  0x05
#define MENU_MESSAGE_TYPE_IDLE  0x06
#define MENU_MESSAGE_TYPE_WTF   0xFF

typedef struct {
    uint8_t type;
    void *data;
} menu_message_t;

typedef struct menu_event_t menu_event_t;

menu_event_t * menu_event_alloc();

void menu_event_free(menu_event_t * menu_event);

void menu_event_activity_notify(menu_event_t * menu_event);

menu_message_t * menu_event_next(menu_event_t * menu_event);

