/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Display backlight control
***/

/*** Includes ***/
#include <notification/notification_messages.h>

#include "backlight_control.h"

/*** Routines ***/

/** Setup the backlight control **/
void set_backlight_control(BacklightControl* blc) {
    /* Enable notifications */
    blc->notifications = furi_record_open(RECORD_NOTIFICATION);
}

/** Release the backlight control **/
void release_backlight_control(void) {
    /* Disable notifications */
    furi_record_close(RECORD_NOTIFICATION);
}

/** Set the backlight on, off or automatic */
void set_backlight(BacklightControl* blc, uint8_t state) {
    /* Set the backlight in the specified state */
    switch(state) {
    case BL_OFF:
        notification_message(blc->notifications, &sequence_display_backlight_off);
        break;

    case BL_ON:
        notification_message(blc->notifications, &sequence_display_backlight_enforce_on);
        break;

    case BL_AUTO:
        notification_message(blc->notifications, &sequence_display_backlight_enforce_auto);
        break;

    default:
        return;
    }
}
