#include "backlight_i.h"

/**
 * @brief    Turns on backlight, even if no user interaction.
*/
void backlight_on() {
    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_on);
    furi_record_close(RECORD_NOTIFICATION);
}

/**
 * @brief    Turns off backlight, unless there is user interaction.
*/
void backlight_off() {
    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);
}

/**
 * @brief    Turns off backlight, even if user interaction.
*/
void backlight_force_off() {
    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_off_delay_1000);
    furi_record_close(RECORD_NOTIFICATION);
}
