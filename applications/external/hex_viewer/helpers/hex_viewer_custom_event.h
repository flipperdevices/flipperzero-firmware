#pragma once

typedef enum {
    HexViewerCustomEventStartscreenUp,
    HexViewerCustomEventStartscreenDown,
    HexViewerCustomEventStartscreenLeft,
    HexViewerCustomEventStartscreenRight,
    HexViewerCustomEventStartscreenOk,
    HexViewerCustomEventStartscreenBack,
    HexViewerCustomEventScene1Up,
    HexViewerCustomEventScene1Down,
    HexViewerCustomEventScene1Left,
    HexViewerCustomEventScene1Right,
    HexViewerCustomEventScene1Ok,
    HexViewerCustomEventScene1Back,
    HexViewerCustomEventScene2Up,
    HexViewerCustomEventScene2Down,
    HexViewerCustomEventScene2Left,
    HexViewerCustomEventScene2Right,
    HexViewerCustomEventScene2Ok,
    HexViewerCustomEventScene2Back,
} HexViewerCustomEvent;

enum HexViewerCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    HexViewerCustomEventMenuVoid,
    HexViewerCustomEventMenuSelected,
    HexViewerCustomEventMenuPercentEntered,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} HexViewerCustomEventMenu;
#pragma pack(pop)

static inline uint32_t hex_viewer_custom_menu_event_pack(uint16_t type, int16_t value) {
    HexViewerCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    hex_viewer_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    HexViewerCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t hex_viewer_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    hex_viewer_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t hex_viewer_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    hex_viewer_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}