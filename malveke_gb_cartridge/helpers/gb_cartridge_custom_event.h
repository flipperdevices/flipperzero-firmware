#pragma once

typedef enum {
    GBCartridgeCustomEventStartscreenUp,
    GBCartridgeCustomEventStartscreenDown,
    GBCartridgeCustomEventStartscreenLeft,
    GBCartridgeCustomEventStartscreenRight,
    GBCartridgeCustomEventStartscreenOk,
    GBCartridgeCustomEventStartscreenBack,
    GBCartridgeCustomEventScene1Up,
    GBCartridgeCustomEventScene1Down,
    GBCartridgeCustomEventScene1Left,
    GBCartridgeCustomEventScene1Right,
    GBCartridgeCustomEventScene1Ok,
    GBCartridgeCustomEventScene1Back,

    GBCartridgeCustomEventScene2Up,
    GBCartridgeCustomEventScene2Down,
    GBCartridgeCustomEventScene2Left,
    GBCartridgeCustomEventScene2Right,
    GBCartridgeCustomEventScene2Ok,
    GBCartridgeCustomEventScene2Back,

    GBCartridgeCustomEventScene3Up,
    GBCartridgeCustomEventScene3Down,
    GBCartridgeCustomEventScene3Left,
    GBCartridgeCustomEventScene3Right,
    GBCartridgeCustomEventScene3Ok,
    GBCartridgeCustomEventScene3Back,

    GBCartridgeCustomEventScene4Up,
    GBCartridgeCustomEventScene4Down,
    GBCartridgeCustomEventScene4Left,
    GBCartridgeCustomEventScene4Right,
    GBCartridgeCustomEventScene4Ok,
    GBCartridgeCustomEventScene4Back,

    GBCartridgeCustomEventScene5Up,
    GBCartridgeCustomEventScene5Down,
    GBCartridgeCustomEventScene5Left,
    GBCartridgeCustomEventScene5Right,
    GBCartridgeCustomEventScene5Ok,
    GBCartridgeCustomEventScene5Back,
} GBCartridgeCustomEvent;

enum GBCartridgeCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    GBCartridgeCustomEventMenuVoid,
    GBCartridgeCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} GBCartridgeCustomEventMenu;
#pragma pack(pop)

static inline uint32_t gb_cartridge_custom_menu_event_pack(uint16_t type, int16_t value) {
    GBCartridgeCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    gb_cartridge_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    GBCartridgeCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t gb_cartridge_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    gb_cartridge_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t gb_cartridge_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    gb_cartridge_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}