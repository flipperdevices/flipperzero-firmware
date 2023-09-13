#pragma once

#include <furi.h>
#include <furi_hal.h>

#include "key_monitor.h"

struct KeyMonitor {
    uint8_t last_pins;
    bool running;
    FuriThread* thread;
    void* context;
    KeyMonitorEventCallback callback;
};
