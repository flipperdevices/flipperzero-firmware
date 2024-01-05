#pragma once

typedef enum {
    Meal_PagerCustomEventStartscreenUp,
    Meal_PagerCustomEventStartscreenDown,
    Meal_PagerCustomEventStartscreenLeft,
    Meal_PagerCustomEventStartscreenRight,
    Meal_PagerCustomEventStartscreenOk,
    Meal_PagerCustomEventStartscreenBack,
    Meal_PagerCustomEventTransmitUp,
    Meal_PagerCustomEventTransmitDown,
    Meal_PagerCustomEventTransmitLeft,
    Meal_PagerCustomEventTransmitRight,
    Meal_PagerCustomEventTransmitOk,
    Meal_PagerCustomEventTransmitBack,
    Meal_PagerCustomEventScene2Up,
    Meal_PagerCustomEventScene2Down,
    Meal_PagerCustomEventScene2Left,
    Meal_PagerCustomEventScene2Right,
    Meal_PagerCustomEventScene2Ok,
    Meal_PagerCustomEventScene2Back,
    Meal_PagerCustomEventViewTransmitterBack,
    Meal_PagerCustomEventViewTransmitterSendStart,
    Meal_PagerCustomEventViewTransmitterSendStop,
    Meal_PagerCustomEventViewTransmitterError,
} Meal_PagerCustomEvent;

enum Meal_PagerCustomEventType {
    // Reserve first 100 events for button types and indexes, starting from 0
    Meal_PagerCustomEventMenuVoid,
    Meal_PagerCustomEventMenuSelected,
};

#pragma pack(push, 1)
typedef union {
    uint32_t packed_value;
    struct {
        uint16_t type;
        int16_t value;
    } content;
} Meal_PagerCustomEventMenu;
#pragma pack(pop)

static inline uint32_t meal_pager_custom_menu_event_pack(uint16_t type, int16_t value) {
    Meal_PagerCustomEventMenu event = {.content = {.type = type, .value = value}};
    return event.packed_value;
}
static inline void
    meal_pager_custom_menu_event_unpack(uint32_t packed_value, uint16_t* type, int16_t* value) {
    Meal_PagerCustomEventMenu event = {.packed_value = packed_value};
    if(type) *type = event.content.type;
    if(value) *value = event.content.value;
}

static inline uint16_t meal_pager_custom_menu_event_get_type(uint32_t packed_value) {
    uint16_t type;
    meal_pager_custom_menu_event_unpack(packed_value, &type, NULL);
    return type;
}

static inline int16_t meal_pager_custom_menu_event_get_value(uint32_t packed_value) {
    int16_t value;
    meal_pager_custom_menu_event_unpack(packed_value, NULL, &value);
    return value;
}