#pragma once

#include <stdint.h>
#include <stddef.h>

enum XRemoteCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    XRemoteCustomEventMenuVoid,
    XRemoteCustomEventMenuSelected,
    XRemoteCustomEventMenuAddSelected,
    XRemoteCustomEventMenuAddIrSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} XRemoteCustomEventMenu;
#pragma pack(pop)

typedef enum {
    XRemoteCustomEventInfoscreenUp,
    XRemoteCustomEventInfoscreenDown,
    XRemoteCustomEventInfoscreenLeft,
    XRemoteCustomEventInfoscreenRight,
    XRemoteCustomEventInfoscreenOk,
    XRemoteCustomEventInfoscreenBack,

    XRemoteCustomEventCreateUp,
    XRemoteCustomEventCreateDown,
    XRemoteCustomEventCreateLeft,
    XRemoteCustomEventCreateRight,
    XRemoteCustomEventCreateOk,
    XRemoteCustomEventCreateBack,

    XRemoteCustomEventScene2Up,
    XRemoteCustomEventScene2Down,
    XRemoteCustomEventScene2Left,
    XRemoteCustomEventScene2Right,
    XRemoteCustomEventScene2Ok,
    XRemoteCustomEventScene2Back,

    XRemoteCustomEventTypePopupClosed,
    XRemoteCustomEventTextInput,

    XRemoteCustomEventPauseSetBack,
    XRemoteCustomEventPauseSetUp,
    XRemoteCustomEventPauseSetDown,
    XRemoteCustomEventPauseSetOk,
} XRemoteCustomEvent;

static inline uint32_t xremote_custom_menu_event_pack(uint16_t type, int16_t value) {
    XRemoteCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}

static inline void
    xremote_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    XRemoteCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t xremote_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    xremote_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t xremote_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    xremote_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}