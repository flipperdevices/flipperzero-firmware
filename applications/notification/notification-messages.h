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

// Message sequences
// Display
extern const NotificationMessage* message_display_on_sequence[];
extern const NotificationMessage* message_display_off_sequence[];

// Led ON
extern const NotificationMessage* message_red_255_sequence[];
extern const NotificationMessage* message_green_255_sequence[];
extern const NotificationMessage* message_blue_255_sequence[];

// Led OFF
extern const NotificationMessage* message_red_0_sequence[];
extern const NotificationMessage* message_green_0_sequence[];
extern const NotificationMessage* message_blue_0_sequence[];
