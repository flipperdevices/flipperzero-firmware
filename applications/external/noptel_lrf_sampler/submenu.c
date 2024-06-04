/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
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

    /* Switch to the sample view */
    case submenu_sample:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_sample);
        app->config.sitem = submenu_sample;
        FURI_LOG_D(TAG, "Switch to sample view");
        break;

    /* Turn the pointer on and off */
    case submenu_pointeronoff:

        /* Start the UART at the correct baudrate */
        start_uart(app->lrf_serial_comm_app, app->config.baudrate);

        /* Send the pointer command */
        send_lrf_command(app->lrf_serial_comm_app, app->pointer_is_on ? pointer_off : pointer_on);

        /* Stop the UART */
        stop_uart(app->lrf_serial_comm_app);

        app->config.sitem = submenu_pointeronoff;
        app->pointer_is_on = !app->pointer_is_on;
        FURI_LOG_D(TAG, "Turned the pointer %s", app->pointer_is_on ? "OFF" : "ON");
        break;

    /* Switch to the LRF info view */
    case submenu_lrfinfo:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_lrfinfo);
        app->config.sitem = submenu_lrfinfo;
        FURI_LOG_D(TAG, "Switch to LRF info view");
        break;

    /* Switch to the save diagnostic view */
    case submenu_savediag:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_savediag);
        app->config.sitem = submenu_savediag;
        FURI_LOG_D(TAG, "Switch to save diagnostic view");
        break;

    /* Switch to the test laser view */
    case submenu_testlaser:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_testlaser);
        app->config.sitem = submenu_testlaser;
        FURI_LOG_D(TAG, "Switch to test laser view");
        break;

    /* Switch to the test pointer view */
    case submenu_testpointer:
        view_dispatcher_switch_to_view(app->view_dispatcher, view_testpointer);
        app->config.sitem = submenu_testpointer;
        FURI_LOG_D(TAG, "Switch to test pointer view");
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
