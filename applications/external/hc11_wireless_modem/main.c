/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Main app
***/

/*** Includes ***/
#include <storage/storage.h>

#include "common.h"
#include "config_save_restore.h"
#include "config_view.h"
#include "passthru_view.h"
#include "about_view.h"
#include "submenu.h"

/*** Routines ***/

/** Initialize the app **/
static App* hc11_modem_app_init() {
    FURI_LOG_I(TAG, "App init");

    /* Allocate space for the app's structure */
    App* app = (App*)malloc(sizeof(App));

    /* Open a GUI instance */
    Gui* gui = furi_record_open(RECORD_GUI);

    /* Allocate space for the view dispatcher */
    app->view_dispatcher = view_dispatcher_alloc();

    /* Configure the view dispatcher */
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    /* Setup the submenu */

    /* Allocate space for the submenu */
    app->submenu = submenu_alloc();

    /* Add submenu items */
    submenu_add_item(
        app->submenu, submenu_item_names[submenu_config], submenu_config, submenu_callback, app);

    submenu_add_item(
        app->submenu,
        submenu_item_names[submenu_passthru],
        submenu_passthru,
        submenu_callback,
        app);

    submenu_add_item(
        app->submenu, submenu_item_names[submenu_about], submenu_about, submenu_callback, app);

    /* Configure the "previous" callback for the submenu, which exits the app */
    view_set_previous_callback(submenu_get_view(app->submenu), submenu_exit_callback);

    /* Add the submenu view */
    view_dispatcher_add_view(app->view_dispatcher, view_submenu, submenu_get_view(app->submenu));

    /* Setup the configuration view */

    /* Allocate space for the configuration list items */
    app->config_list = variable_item_list_alloc();
    variable_item_list_reset(app->config_list);

    /* Add channel option list item */
    app->item_channel = variable_item_list_add(
        app->config_list,
        config_channel_label,
        nb_config_channel_values,
        config_channel_change,
        app);

    /* Add address option list item */
    app->item_address = variable_item_list_add(
        app->config_list,
        config_address_label,
        nb_config_address_values,
        config_address_change,
        app);

    /* Add address option list item */
    app->item_txpower = variable_item_list_add(
        app->config_list,
        config_txpower_label,
        nb_config_txpower_values,
        config_txpower_change,
        app);

    /* Add mode option list item */
    app->item_mode = variable_item_list_add(
        app->config_list, config_mode_label, nb_config_mode_values, config_mode_change, app);

    /* Add USB passthrough channel option list item */
    app->item_passthru_chan = variable_item_list_add(
        app->config_list,
        config_passthru_chan_label,
        nb_config_passthru_chan_values,
        config_passthru_chan_change,
        app);

    /* Configure the "previous" callback for the configuration view */
    view_set_previous_callback(
        variable_item_list_get_view(app->config_list), return_to_submenu_callback);

    /* Add the configuration view */
    view_dispatcher_add_view(
        app->view_dispatcher, view_config, variable_item_list_get_view(app->config_list));

    /* Setup the USB serial passthrough */

    /* Allocate space for the passthrough view */
    app->passthru_view = view_alloc();

    /* Setup the draw callback for the passthrough view */
    view_set_draw_callback(app->passthru_view, passthru_view_draw_callback);

    /* Setup the input callback for the passthrough view */
    view_set_input_callback(app->passthru_view, passthru_view_input_callback);

    /* Configure the "previous" callback for the passthrough view */
    view_set_previous_callback(app->passthru_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the passthrough view */
    view_set_enter_callback(app->passthru_view, passthru_view_enter_callback);
    view_set_exit_callback(app->passthru_view, passthru_view_exit_callback);

    /* Set the context for the passthrough view callbacks */
    view_set_context(app->passthru_view, app);

    /* Allocate space for the USB serial passthrough view model */
    view_allocate_model(app->passthru_view, ViewModelTypeLockFree, sizeof(PassthruModel));

    /* Add the passthrough view */
    view_dispatcher_add_view(app->view_dispatcher, view_passthru, app->passthru_view);

    /* Setup the about view */

    /* Allocate space for the about view */
    app->about_view = view_alloc();

    /* Setup the draw callback for the about view */
    view_set_draw_callback(app->about_view, about_view_draw_callback);

    /* Setup the input callback for the about view */
    view_set_input_callback(app->about_view, about_view_input_callback);

    /* Configure the "previous" callback for the about view */
    view_set_previous_callback(app->about_view, return_to_submenu_callback);

    /* Configure the enter callback for the about view */
    view_set_enter_callback(app->about_view, about_view_enter_callback);

    /* Set the context for the about view callbacks */
    view_set_context(app->about_view, app);

    /* Allocate space for the about view model */
    view_allocate_model(app->about_view, ViewModelTypeLockFree, sizeof(AboutModel));

    /* Add the about view */
    view_dispatcher_add_view(app->view_dispatcher, view_about, app->about_view);

    /* Start out at the submenu view */
    view_dispatcher_switch_to_view(app->view_dispatcher, view_submenu);

    /* Setup the default configuration */

    /* Set the default channel option */
    app->config.channel = config_channel_values[0];
    variable_item_set_current_value_index(app->item_channel, 0);
    variable_item_set_current_value_text(app->item_channel, config_channel_names[0]);

    /* Set the default address option */
    app->config.address = config_address_values[0];
    variable_item_set_current_value_index(app->item_address, 0);
    variable_item_set_current_value_text(app->item_address, config_address_names[0]);

    /* Set the default TX power option - i.e. power level 4 */
    app->config.txpower = config_txpower_values[3];
    variable_item_set_current_value_index(app->item_txpower, 3);
    variable_item_set_current_value_text(app->item_txpower, config_txpower_names[3]);

    /* Set the default mode option - i.e. mode FU3 */
    app->config.mode = config_mode_values[1];
    variable_item_set_current_value_index(app->item_mode, 1);
    variable_item_set_current_value_text(app->item_mode, config_mode_names[1]);

    /* Set the default USB passthrough channel option */
    app->config.passthru_chan = config_passthru_chan_values[0];
    variable_item_set_current_value_index(app->item_passthru_chan, 0);
    variable_item_set_current_value_text(app->item_passthru_chan, config_passthru_chan_names[0]);

    /* Set the default submenu item */
    app->config.sitem = submenu_config;

    /* Try to load the configuration file and restore the configuration from the
     saved values */
    load_configuration(app);

    /* Setup the backlight control */
    set_backlight_control(&app->backlight_control);

    /* Initialize the serial communication app */
    app->hc11_radio_comm_app = hc11_radio_comm_app_init(min_led_flash_duration);

    return app;
}

/** Free up the space allocated for the app **/
static void hc11_modem_app_free(App* app) {
    FURI_LOG_I(TAG, "App free");

    /* Stop and free up the serial communication app */
    hc11_radio_comm_app_free(app->hc11_radio_comm_app);

    /* Release the backlight control */
    release_backlight_control();

    /* Try to save the configuration */
    save_configuration(app);

    /* Remove the about view */
    view_dispatcher_remove_view(app->view_dispatcher, view_about);
    view_free(app->about_view);

    /* Remove the USB serial passthrough view */
    view_dispatcher_remove_view(app->view_dispatcher, view_passthru);
    view_free(app->passthru_view);

    /* Remove the configuration view */
    view_dispatcher_remove_view(app->view_dispatcher, view_config);
    variable_item_list_free(app->config_list);

    /* Remove the submenu view */
    view_dispatcher_remove_view(app->view_dispatcher, view_submenu);

    /* Free the submenu */
    submenu_free(app->submenu);

    /* Free the view dispatcher */
    view_dispatcher_free(app->view_dispatcher);

    /* Close the GUI instance */
    furi_record_close(RECORD_GUI);

    /* Free the app's structure */
    free(app);
}

/** App entry point **/
int32_t hc11_modem_app_entry(void* p) {
    UNUSED(p);

    /* Initialize the app */
    App* app = hc11_modem_app_init();

    /* Run the view dispatcher */
    FURI_LOG_D(TAG, "Run view dispatcher");
    view_dispatcher_run(app->view_dispatcher);

    /* Free up the space for the app */
    hc11_modem_app_free(app);

    return 0;
}
