#include "backlight_i.h"

struct Backlight {
    bool backlight_on_setting;
};

/**
 * @brief      Allocates a new Backlight object.
 * @details    Creates a new Backlight object.  The Backlight object is responsible for
 *            controlling the backlight.  You can turn the backlight on, or turn it back
 *            off (unless user interaction).
 * @return     Pointer to Backlight object.
*/
Backlight* backlight_alloc() {
    Backlight* backlight = malloc(sizeof(Backlight));
    backlight->backlight_on_setting = false;
    return backlight;
}

/**
 * @brief      Frees a Backlight object.
 * @details    Frees a Backlight object.
 * @param      backlight  Pointer to Backlight object.
*/
void backlight_free(Backlight* backlight) {
    if(backlight) {
        if(backlight->backlight_on_setting) {
            backlight_off(backlight);
        }
        free(backlight);
    }
}

/**
 * @brief    Turns on backlight, even if no user interaction.
 * @param    backlight  Pointer to Backlight object.
*/
void backlight_on(Backlight* backlight) {
    if(!backlight || backlight->backlight_on_setting) {
        return;
    }

    backlight->backlight_on_setting = true;

    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_on);
    furi_record_close(RECORD_NOTIFICATION);
}

/**
 * @brief    Turns off backlight, unless there is user interaction.
 * @param    backlight  Pointer to Backlight object.
*/
void backlight_off(Backlight* backlight) {
    if(!backlight || !backlight->backlight_on_setting) {
        return;
    }

    backlight->backlight_on_setting = false;

    notification_message(
        furi_record_open(RECORD_NOTIFICATION), &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);
}
