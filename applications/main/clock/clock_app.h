#pragma once

#include <input/input.h>
#include "clock_settings.h"

#define TAG "Clock"

#define CLOCK_ISO_DATE_FORMAT "%.4d-%.2d-%.2d"
#define CLOCK_RFC_DATE_FORMAT "%.2d-%.2d-%.4d"
#define CLOCK_TIME_FORMAT "%.2d:%.2d:%.2d"

#define MERIDIAN_FORMAT "%s"
#define MERIDIAN_STRING_AM "AM"
#define MERIDIAN_STRING_PM "PM"

#define TIME_LEN 12
#define DATE_LEN 14
#define MERIDIAN_LEN 3

#define DEFAULT_SETTINGS                                                                       \
    (ClockSettings) {                                                                          \
        .time_format = H12, .date_format = Iso, .increment_precision = 5, .alarm_mode = Sound, \
        .alarm_sound = 0                                                                       \
    }

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
