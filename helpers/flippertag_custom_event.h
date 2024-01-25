#pragma once

typedef enum {
    FlipperTagCustomEventStartscreenUp,
    FlipperTagCustomEventStartscreenDown,
    FlipperTagCustomEventStartscreenLeft,
    FlipperTagCustomEventStartscreenRight,
    FlipperTagCustomEventStartscreenOk,
    FlipperTagCustomEventStartscreenBack,
    FlipperTagCustomEventScene1Up,
    FlipperTagCustomEventScene1Down,
    FlipperTagCustomEventScene1Left,
    FlipperTagCustomEventScene1Right,
    FlipperTagCustomEventScene1Ok,
    FlipperTagCustomEventScene1Back,
} FlipperTagCustomEvent;

enum FlipperTagCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    FlipperTagCustomEventMenuVoid,
    FlipperTagCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} FlipperTagCustomEventMenu;
#pragma pack(pop)

static inline uint32_t flippertag_custom_menu_event_pack(uint16_t type, int16_t value) {
    FlipperTagCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void flippertag_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    FlipperTagCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t flippertag_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    flippertag_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t flippertag_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    flippertag_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}