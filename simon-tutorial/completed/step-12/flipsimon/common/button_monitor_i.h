#pragma once

#include <furi.h>
#include <furi_hal.h>

#include "button_monitor.h"

// How long to wait after initial press is detected before sampling the switch.
// TODO: Can we decrease this so we are most responsive?
#define DEBOUNCE_WAIT_MS 50

// How many samples need to be the same before the switch is considered debounced.
#define DEBOUNCE_SAME_MIN_COUNT 100

struct ButtonMonitor {
    // GPIO state from previous scan.
    SwitchIds last_pins;

    // true if the monitor is running.
    bool running;

    // The thread that runs the monitor.
    FuriThread* thread;

    // The callback to call when a button event occurs.
    ButtonMonitorEventCallback callback;

    // The context for the callback.
    void* context;
};

static int32_t button_monitor_worker(void* context);
