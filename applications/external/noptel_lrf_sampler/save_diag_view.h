/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Save diagnostic view
***/

/*** Routines ***/

/** Save diagnostic view enter callback **/
void savediag_view_enter_callback(void*);

/** Save diagnostic view exit callback **/
void savediag_view_exit_callback(void*);

/** Draw callback for the save diagnostic view **/
void savediag_view_draw_callback(Canvas*, void*);

/** Input callback for the save diagnostic view **/
bool savediag_view_input_callback(InputEvent*, void*);
