#pragma once

#include <input/input.h>
#include "clock_settings.h"

#define TAG "Clock"
#define CLOCK_ISO_DATE_FORMAT "%.4d-%.2d-%.2d"
#define CLOCK_RFC_DATE_FORMAT "%.2d-%.2d-%.4d"
#define CLOCK_TIME_FORMAT "%.2d:%.2d:%.2d"
#define MAX_LEN 20

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

// typedef enum {
//     Clock,
//     Timer,
//     Counter,
// } Mode;

typedef struct {
    ClockSettings settings;
    FuriHalRtcDateTime datetime;
    FuriMutex* mutex;
    FuriMessageQueue* event_queue;
} ClockState;
