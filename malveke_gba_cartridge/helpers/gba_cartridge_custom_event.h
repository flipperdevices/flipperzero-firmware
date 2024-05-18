#pragma once

typedef enum {
    GBACartridgeCustomEventStartscreenUp,
    GBACartridgeCustomEventStartscreenDown,
    GBACartridgeCustomEventStartscreenLeft,
    GBACartridgeCustomEventStartscreenRight,
    GBACartridgeCustomEventStartscreenOk,
    GBACartridgeCustomEventStartscreenBack,
    GBACartridgeCustomEventScene1Up,
    GBACartridgeCustomEventScene1Down,
    GBACartridgeCustomEventScene1Left,
    GBACartridgeCustomEventScene1Right,
    GBACartridgeCustomEventScene1Ok,
    GBACartridgeCustomEventScene1Back

} GBACartridgeCustomEvent;

enum GBACartridgeCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    GBACartridgeCustomEventMenuVoid,
    GBACartridgeCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} GBACartridgeCustomEventMenu;
#pragma pack(pop)

static inline uint32_t gba_cartridge_custom_menu_event_pack(uint16_t type, int16_t value) {
    GBACartridgeCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    gba_cartridge_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    GBACartridgeCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t gba_cartridge_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    gba_cartridge_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t gba_cartridge_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    gba_cartridge_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}