/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * USB serial passthrough view
***/

/*** Routines ***/

/** USB serial passthrough view enter callback
    Configure the virtual COM port and start the RX/TX thread */
void passthru_view_enter_callback(void*);

/** USB serial passthrough view exit callback
    Stop the virtual COM port RX/TX thread and release the virtual COM port **/
void passthru_view_exit_callback(void*);

/** Draw callback for the USB serial passthrough view **/
void passthru_view_draw_callback(Canvas*, void*);

/** Input callback for the USB serial passthrough view **/
bool passthru_view_input_callback(InputEvent*, void*);
