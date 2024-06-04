/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Sample view
***/

/*** Routines ***/

/** Sample view enter callback
    Setup the timer to update the sample view regularly **/
void sample_view_enter_callback(void*);

/** Sample view exit callback
    Stop the timer to update the sample view **/
void sample_view_exit_callback(void*);

/** Draw callback for the sample view **/
void sample_view_draw_callback(Canvas*, void*);

/** Input callback for the sample view **/
bool sample_view_input_callback(InputEvent*, void*);
