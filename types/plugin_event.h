#ifndef _TOTP_PLUGIN_EVENT_H_
#define _TOTP_PLUGIN_EVENT_H_

#include <inttypes.h>
#include <input/input.h>
#include "event_type.h"

typedef struct {
    EventType type;
    InputEvent input;
} PluginEvent;

#endif
