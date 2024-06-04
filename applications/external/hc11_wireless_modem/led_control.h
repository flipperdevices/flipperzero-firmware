/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * LED control
***/

/*** Includes ***/
#include <notification/notification.h>

/*** Defines ***/
#define RED 1
#define GREEN 2
#define BLUE 3

/*** Types ***/
typedef struct {
    /* App notifications */
    NotificationApp* notifications;

    /* Minimum LED flashing duration */
    uint16_t min_led_flash_duration;

    /* Timer callback to turn off the LED */
    FuriTimer* led_off_timer;

} LEDControl;

/*** Routines ***/

/** Setup the LED control **/
void set_led_control(LEDControl*, uint16_t);

/** Release the LED control **/
void release_led_control(LEDControl*);

/** Start a LED flash of a certain color and schedule its extinction */
void start_led_flash(LEDControl*, uint8_t);
