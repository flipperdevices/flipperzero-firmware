#include "notification.h"
#include <stddef.h>

// Messages
// Display
const NotificationMessage message_display_on = {
    .type = NotificationMessageTypeDisplay,
    .data.display.on = true,
};

const NotificationMessage message_display_off = {
    .type = NotificationMessageTypeDisplay,
    .data.display.on = false,
};

// Led ON
const NotificationMessage message_red_255 = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = 0xFF,
};

const NotificationMessage message_green_255 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0xFF,
};

const NotificationMessage message_blue_255 = {
    .type = NotificationMessageTypeLedBlue,
    .data.led.value = 0xFF,
};

// Led OFF
const NotificationMessage message_red_0 = {
    .type = NotificationMessageTypeLedRed,
    .data.led.value = 0x00,
};

const NotificationMessage message_green_0 = {
    .type = NotificationMessageTypeLedGreen,
    .data.led.value = 0x00,
};

const NotificationMessage message_blue_0 = {
    .type = NotificationMessageTypeLedBlue,
    .data.led.value = 0x00,
};

// Message sequences
// Display
const NotificationMessage* message_display_on_sequence[] = {
    &message_display_on,
    NULL,
};

const NotificationMessage* message_display_off_sequence[] = {
    &message_display_off,
    NULL,
};

// Led ON
const NotificationMessage* message_red_255_sequence[] = {
    &message_red_255,
    NULL,
};

const NotificationMessage* message_green_255_sequence[] = {
    &message_green_255,
    NULL,
};

const NotificationMessage* message_blue_255_sequence[] = {
    &message_blue_255,
    NULL,
};

// Led OFF
const NotificationMessage* message_red_0_sequence[] = {
    &message_red_0,
    NULL,
};

const NotificationMessage* message_green_0_sequence[] = {
    &message_green_0,
    NULL,
};

const NotificationMessage* message_blue_0_sequence[] = {
    &message_blue_0,
    NULL,
};

// Charging
const NotificationMessage* message_charging_sequence[] = {
    &message_red_255,
    &message_green_0,
    NULL,
};

const NotificationMessage* message_charged_sequence[] = {
    &message_green_255,
    &message_red_0,
    NULL,
};

const NotificationMessage* message_not_charging_sequence[] = {
    &message_red_0,
    &message_green_0,
    NULL,
};