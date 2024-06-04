/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Display backlight control
***/

/*** Includes ***/
#include <notification/notification.h>

/*** Defines ***/
#define BL_OFF 0
#define BL_ON 1
#define BL_AUTO 2

/*** Types ***/
typedef struct {
    /* App notifications */
    NotificationApp* notifications;

} BacklightControl;

/*** Routines ***/

/** Setup the backlight control **/
void set_backlight_control(BacklightControl*);

/** Release the backlight control **/
void release_backlight_control(void);

/** Set the backlight on, off or automatic */
void set_backlight(BacklightControl*, uint8_t);
