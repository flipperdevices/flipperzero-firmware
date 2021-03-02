#pragma once

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>

typedef enum { EventTypeTick, EventTypeKey, EventTypeRx } EventType;

typedef struct {
    uint8_t dummy;
} RxEvent;

typedef struct {
    union {
        InputEvent input;
        RxEvent rx;
    } value;
    EventType type;
} AppEvent;
