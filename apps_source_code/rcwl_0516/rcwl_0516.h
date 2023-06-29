#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <notification/notification_messages.h>

struct Rcwl_0516 {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    const GpioPin* input_pin;
    const GpioPin* output_pin;

    bool input_value;
    bool output_value;

    NotificationApp* notifications;

};

typedef struct Rcwl_0516 Rcwl_0516;