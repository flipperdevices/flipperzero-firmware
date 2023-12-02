#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>

typedef struct BatteryCheckerApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    bool test;
    float voltage;
} BatteryCheckerApp;
