#ifndef _TOTP_EVENT_TYPE_H_
#define _TOTP_EVENT_TYPE_H_

#include <inttypes.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

#endif
