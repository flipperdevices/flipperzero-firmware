#pragma once

#include <furi.h>
#include <api-hal.h>
#include <gui/gui.h>

typedef enum { EventTypeTick, EventTypeKey, EventTypeRx } EventType;

typedef enum { ProtocolEm4100, ProtocolHid } Protocol;

typedef struct {
    uint8_t dummy;
    Protocol protocol;
} RxEvent;

typedef struct {
    union {
        InputEvent input;
        RxEvent rx;
    } value;
    EventType type;
} AppEvent;
