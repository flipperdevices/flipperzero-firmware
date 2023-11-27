#pragma once

#include <furi.h>
#include <furi_hal.h>

#include "button_monitor.h"

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
