#pragma once

typedef enum {
    BoilerplateCustomEventStartscreenUp,
    BoilerplateCustomEventStartscreenDown,
    BoilerplateCustomEventStartscreenLeft,
    BoilerplateCustomEventStartscreenRight,
    BoilerplateCustomEventStartscreenOk,
    BoilerplateCustomEventStartscreenBack,
    BoilerplateCustomEventScene1Up,
    BoilerplateCustomEventScene1Down,
    BoilerplateCustomEventScene1Left,
    BoilerplateCustomEventScene1Right,
    BoilerplateCustomEventScene1Ok,
    BoilerplateCustomEventScene1Back,
    BoilerplateCustomEventScene2Up,
    BoilerplateCustomEventScene2Down,
    BoilerplateCustomEventScene2Left,
    BoilerplateCustomEventScene2Right,
    BoilerplateCustomEventScene2Ok,
    BoilerplateCustomEventScene2Back,
} BoilerplateCustomEvent;

enum BoilerplateCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    BoilerplateCustomEventMenuVoid,
    BoilerplateCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} BoilerplateCustomEventMenu;
#pragma pack(pop)

static inline uint32_t boilerplate_custom_menu_event_pack(uint16_t type, int16_t value) {
    BoilerplateCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    boilerplate_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    BoilerplateCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t boilerplate_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    boilerplate_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t boilerplate_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    boilerplate_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}