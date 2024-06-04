/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * LRF information view
***/

/*** Routines ***/

/** LRF info view enter callback **/
void lrfinfo_view_enter_callback(void*);

/** LRF info view exit callback **/
void lrfinfo_view_exit_callback(void*);

/** Draw callback for the LRF info view **/
void lrfinfo_view_draw_callback(Canvas*, void*);

/** Input callback for the LRF info view **/
bool lrfinfo_view_input_callback(InputEvent*, void*);
