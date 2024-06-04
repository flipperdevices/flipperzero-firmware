/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Submenu
***/

/** Views **/
typedef enum {

    /* Submenu */
    view_submenu,

    /* Configuration view */
    view_config,

    /* Sample view */
    view_sample,

    /* LRF info view */
    view_lrfinfo,

    /* Save diagnostic view */
    view_savediag,

    /* Test laser view */
    view_testlaser,

    /* Test pointer view */
    view_testpointer,

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
