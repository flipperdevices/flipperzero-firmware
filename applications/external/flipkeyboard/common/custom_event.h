/**
 * @file    custom_event.h
 * @brief   A collection of CustomEventIds.
 * @details This file contains the custom event ids for the  application.  Register for events using:
 * view_dispatcher_set_custom_event_callback(flipboard_get_view_dispatcher(app), custom_event_handler);
*/

#pragma once

typedef enum {
    CustomEventFlipboardButtonPress = 0x0000, // FlipboardButton was pressed.
    CustomEventAppMenuEnter = 0x2001, // AppMenu was entered.
    CustomEventAppMenuExit = 0x2002, // AppMenu was exited.
} CustomEventIds;