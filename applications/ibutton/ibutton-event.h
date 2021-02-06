#pragma once
#include <stdint.h>

class iButtonApp;

class iButtonEvent {
public:
    // events enum
    enum class Type : uint8_t {
        EventTypeTick,
        EventTypeBack,
        EventTypeMenuSelected,
    };

    // payload
    union {
        uint32_t menu_index;
    } payload;

    // event type
    Type type;
};
