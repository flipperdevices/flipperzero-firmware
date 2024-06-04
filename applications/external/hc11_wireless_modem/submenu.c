/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Submenu
***/

/*** Includes ***/
#include "common.h"
#include "submenu.h"

/*** Routines ***/

/** Handle submenu views switching **/
void submenu_callback(void* ctx, uint32_t idx) {
    App* app = (App*)ctx;

    switch(idx) {
    /* Switch to the configuration view */
    case submenu_config:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_config);
        app->config.sitem = submenu_config;
        FURI_LOG_D(TAG, "Switch to configuration view");
        break;

    /* Switch to the USB serial passthrough view */
    case submenu_passthru:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_passthru);
        app->config.sitem = submenu_passthru;
        FURI_LOG_D(TAG, "Switch to USB serial passthrough view");
        break;

    /* Switch to the about view */
    case submenu_about:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_about);
        app->config.sitem = submenu_about;
        FURI_LOG_D(TAG, "Switch to about view");
        break;

    default:
        break;
    }
}

/** Callback to return to the submenu **/
uint32_t return_to_submenu_callback(void* ctx) {
    UNUSED(ctx);

    /* Switch back to the submenu */
    FURI_LOG_D(TAG, "Switch back to submenu");
    return view_submenu;
}

/* Callback to exit the submenu and the app altogether */
uint32_t submenu_exit_callback(void* ctx) {
    UNUSED(ctx);

    /* Set the view to VIEW_NONE to exit */
    FURI_LOG_D(TAG, "Exit");
    return VIEW_NONE;
}
