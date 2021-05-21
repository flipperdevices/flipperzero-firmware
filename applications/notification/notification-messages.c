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

// Delay
const NotificationMessage message_delay_100 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 100,
};

// Message sequences
// Display
const NotificationSequence sequence_display_on = {
    &message_display_on,
    NULL,
};

const NotificationSequence sequence_display_off = {
    &message_display_off,
    NULL,
};

// Charging
const NotificationSequence sequence_charging = {
    &message_red_255,
    &message_green_0,
    NULL,
};

const NotificationSequence sequence_charged = {
    &message_green_255,
    &message_red_0,
    NULL,
};

const NotificationSequence sequence_not_charging = {
    &message_red_0,
    &message_green_0,
    NULL,
};

// Blink
const NotificationSequence sequence_blink_red_100 = {
    &message_red_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_green_100 = {
    &message_green_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_blue_100 = {
    &message_blue_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_yellow_100 = {
    &message_red_255,
    &message_green_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_cyan_100 = {
    &message_green_255,
    &message_blue_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_magenta_100 = {
    &message_red_255,
    &message_blue_255,
    &message_delay_100,
    NULL,
};

const NotificationSequence sequence_blink_white_100 = {
    &message_red_255,
    &message_green_255,
    &message_blue_255,
    &message_delay_100,
    NULL,
};