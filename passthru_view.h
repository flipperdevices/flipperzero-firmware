/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.6
 *
 * USB serial passthrough view
***/

/*** Routines ***/

/** USB serial passthrough view enter callback
    Setup the timer to update the passthrough view regularly **/
void passthru_view_enter_callback(void *);

/** USB serial passthrough view exit callback
    Stop the timer to update the passthrough view **/
void passthru_view_exit_callback(void *);

/** Draw callback for the USB serial passthrough view
    Stop the timer to update the passthrough view **/
void passthru_view_draw_callback(Canvas *, void *);

/** Input callback for the USB serial passthrough view **/
bool passthru_view_input_callback(InputEvent *, void *);
