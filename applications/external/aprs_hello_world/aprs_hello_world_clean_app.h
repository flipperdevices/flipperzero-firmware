#pragma once

#include <furi.h>
#include <gui/gui.h>

#include <furi_hal.h>
#include <furi_hal_pwm.h>

#include <notification/notification_messages.h>

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} APRSHelloWorldCleanEvent;

struct APRSHelloWorldCleanApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;

    FuriTimer* timer;

    NotificationApp* notifications;

    const GpioPin* output_pin;

    bool output_value;
};

typedef struct APRSHelloWorldCleanApp APRSHelloWorldCleanApp;

const NotificationSequence red_led_enable = {

    &message_red_255,
    &message_delay_500,

    NULL,
};

const NotificationSequence red_led_disable = {

    &message_red_0,

    NULL,
};

const NotificationSequence blue_led_enable = {

    &message_blue_255,
    &message_delay_250,
    &message_blue_0,

    NULL,
};
