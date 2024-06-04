/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * LED control
***/

/*** Includes ***/
#include <notification/notification_messages.h>

#include "led_control.h"

/*** Routines ***/

/** Timer callback to turn off the LED **/
static void led_off_timer_callback(void* ctx) {
    LEDControl* ldc = (LEDControl*)ctx;

    notification_message(ldc->notifications, &sequence_reset_rgb);
}

/** Setup the LED control **/
void set_led_control(LEDControl* ldc, uint16_t min_led_flash_duration) {
    /* Configure the minimum LED flashing duration */
    ldc->min_led_flash_duration = min_led_flash_duration;

    /* Setup the timer to turn off the LED */
    ldc->led_off_timer = furi_timer_alloc(led_off_timer_callback, FuriTimerTypeOnce, ldc);

    /* Enable notifications */
    ldc->notifications = furi_record_open(RECORD_NOTIFICATION);
}

/** Release the LED control **/
void release_led_control(LEDControl* ldc) {
    /* Stop and free the timer callback to turn off the LED */
    furi_timer_stop(ldc->led_off_timer);
    furi_timer_free(ldc->led_off_timer);

    /* Make sure the LED is turned off */
    notification_message(ldc->notifications, &sequence_reset_rgb);

    /* Disable notifications */
    furi_record_close(RECORD_NOTIFICATION);
}

/** Start a LED flash of a certain color and schedule its extinction */
void start_led_flash(LEDControl* ldc, uint8_t color) {
    /* Turn on the red led if required */
    switch(color) {
    case RED:
        notification_message(ldc->notifications, &sequence_set_only_red_255);
        break;

    case GREEN:
        notification_message(ldc->notifications, &sequence_set_only_green_255);
        break;

    case BLUE:
        notification_message(ldc->notifications, &sequence_set_only_blue_255);
        break;

    default:
        return;
    }

    /* Schedule the LED's extinction */
    furi_timer_start(ldc->led_off_timer, ldc->min_led_flash_duration);
}
