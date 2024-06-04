/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Submenu
***/

/** Views **/
typedef enum {

    /* Submenu */
    view_submenu,

    /* Configuration view */
    view_config,

    /* USB serial passthrough view */
    view_passthru,

    /* About view */
    view_about,

} AppView;

/*** Routines ***/

/** Handle submenu views switching **/
void submenu_callback(void*, uint32_t);

/** Callback to return to the submenu **/
uint32_t return_to_submenu_callback(void*);

/* Callback to exit the submenu and the app altogether */
uint32_t submenu_exit_callback(void*);
