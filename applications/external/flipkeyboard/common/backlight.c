#include "backlight_i.h"

static bool backlight_on_setting = false;

/**
 * @brief    Turns on backlight, even if no user interaction.
*/
void backlight_on() {
    if(backlight_on_setting) {
        return;
    }

    backlight_on_setting = true;

    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_on);
    furi_record_close(RECORD_NOTIFICATION);
}

/**
 * @brief    Turns off backlight, unless there is user interaction.
*/
void backlight_off() {
    if(!backlight_on_setting) {
        return;
    }

    backlight_on_setting = false;

    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);
}
