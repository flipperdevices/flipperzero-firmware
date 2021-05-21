#pragma once
#include "notification.h"

// Messages
// Display
extern const NotificationMessage message_display_on;
extern const NotificationMessage message_display_off;

// Led ON
extern const NotificationMessage message_red_255;
extern const NotificationMessage message_green_255;
extern const NotificationMessage message_blue_255;

// Led OFF
extern const NotificationMessage message_red_0;
extern const NotificationMessage message_green_0;
extern const NotificationMessage message_blue_0;

// Delay
extern const NotificationMessage message_delay_100;

// Message sequences
// Display
extern const NotificationSequence sequence_display_on;
extern const NotificationSequence sequence_display_off;

// Charging
extern const NotificationSequence sequence_charging;
extern const NotificationSequence sequence_charged;
extern const NotificationSequence sequence_not_charging;

// Blink
extern const NotificationSequence sequence_blink_red_100;
extern const NotificationSequence sequence_blink_green_100;
extern const NotificationSequence sequence_blink_blue_100;
extern const NotificationSequence sequence_blink_yellow_100;
extern const NotificationSequence sequence_blink_cyan_100;
extern const NotificationSequence sequence_blink_magenta_100;
extern const NotificationSequence sequence_blink_white_100;