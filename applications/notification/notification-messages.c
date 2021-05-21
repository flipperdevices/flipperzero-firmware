#include "notification.h"
#include "notification-messages-notes.h"
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
const NotificationMessage message_delay_10 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 10,
};

const NotificationMessage message_delay_25 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 25,
};

const NotificationMessage message_delay_50 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 50,
};

const NotificationMessage message_delay_100 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 100,
};

const NotificationMessage message_delay_250 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 250,
};

const NotificationMessage message_delay_500 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 500,
};

const NotificationMessage message_delay_1000 = {
    .type = NotificationMessageTypeDelay,
    .data.delay.length = 1000,
};

// Sound
const NotificationMessage message_sound_off = {
    .type = NotificationMessageTypeSoundOff,
};

// Vibro
const NotificationMessage message_vibro_on = {
    .type = NotificationMessageTypeVibro,
    .data.vibro.on = true,
};

const NotificationMessage message_vibro_off = {
    .type = NotificationMessageTypeVibro,
    .data.vibro.on = false,
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
const NotificationSequence sequence_blink_red_10 = {
    &message_red_255,
    &message_delay_10,
    NULL,
};

const NotificationSequence sequence_blink_green_10 = {
    &message_green_255,
    &message_delay_10,
    NULL,
};

const NotificationSequence sequence_blink_yellow_10 = {
    &message_red_255,
    &message_green_255,
    &message_delay_10,
    NULL,
};

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

// General
const NotificationSequence sequence_success = {
    &message_green_255,
    &message_vibro_on,
    &message_note_a6,
    &message_delay_50,
    &message_vibro_off,
    &message_sound_off,
    &message_delay_500,
    NULL,
};

const NotificationSequence sequence_error = {
    &message_red_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_delay_100,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_delay_500,
    NULL,
};