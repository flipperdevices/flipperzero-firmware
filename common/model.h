#pragma once

#include <furi.h>

typedef bool (*KeyMonitorEventCallback)(void* context, uint8_t old_key, uint8_t new_key);
typedef struct {
    uint8_t last_pins;
    bool running;
    FuriThread* thread;
    void* context;
    KeyMonitorEventCallback callback;
} KeyMonitor;

typedef struct {
    uint32_t setting_1_index;
    uint32_t setting_2_index;

    uint8_t setting_sw1up_index;
    uint8_t setting_sw1down_index;
    uint8_t setting_sw2up_index;
    uint8_t setting_sw2down_index;
    uint8_t setting_sw3up_index;
    uint8_t setting_sw3down_index;
    uint8_t setting_sw4up_index;
    uint8_t setting_sw4down_index;

    uint8_t setting_key1_key_index[10];
    uint8_t setting_key1_count_index[10];

    void* keyboard;
    void* leds;
    KeyMonitor* key_monitor;
    bool has_speaker;
} FlipboardModel;