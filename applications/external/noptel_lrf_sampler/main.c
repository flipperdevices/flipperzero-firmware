/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Main app
***/

/*** Includes ***/
#include <storage/storage.h>

#include "common.h"
#include "config_save_restore.h"
#include "lrf_power_control.h"
#include "config_view.h"
#include "sample_view.h"
#include "lrf_info_view.h"
#include "save_diag_view.h"
#include "test_laser_view.h"
#include "test_pointer_view.h"
#include "passthru_view.h"
#include "about_view.h"
#include "submenu.h"

/*** Routines ***/

/** Initialize the app **/
static App* noptel_lrf_sampler_app_init() {
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
        app->submenu, submenu_item_names[submenu_sample], submenu_sample, submenu_callback, app);

    submenu_add_item(
        app->submenu,
        submenu_item_names[submenu_pointeronoff],
        submenu_pointeronoff,
        submenu_callback,
        app);

    submenu_add_item(
        app->submenu, submenu_item_names[submenu_lrfinfo], submenu_lrfinfo, submenu_callback, app);

    submenu_add_item(
        app->submenu,
        submenu_item_names[submenu_savediag],
        submenu_savediag,
        submenu_callback,
        app);

    submenu_add_item(
        app->submenu,
        submenu_item_names[submenu_testlaser],
        submenu_testlaser,
        submenu_callback,
        app);

    submenu_add_item(
        app->submenu,
        submenu_item_names[submenu_testpointer],
        submenu_testpointer,
        submenu_callback,
        app);

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

    /* Add sampling mode setting list items */
    app->item_mode = variable_item_list_add(
        app->config_list, config_mode_label, nb_config_mode_values, config_mode_change, app);

    /* Add buffering setting list items */
    app->item_buf = variable_item_list_add(
        app->config_list, config_buf_label, nb_config_buf_values, config_buf_change, app);

    /* Add beep option list items */
    app->item_beep = variable_item_list_add(
        app->config_list, config_beep_label, nb_config_beep_values, config_beep_change, app);

    /* Add baudrate option list items */
    app->item_baudrate = variable_item_list_add(
        app->config_list,
        config_baudrate_label,
        nb_config_baudrate_values,
        config_baudrate_change,
        app);

    /* Add USB passthrough channel option list items */
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

    /* Setup the sample view */

    /* Allocate space for the sample view */
    app->sample_view = view_alloc();

    /* Setup the draw callback for the sample view */
    view_set_draw_callback(app->sample_view, sample_view_draw_callback);

    /* Setup the input callback for the sample view */
    view_set_input_callback(app->sample_view, sample_view_input_callback);

    /* Configure the "previous" callback for the sample view */
    view_set_previous_callback(app->sample_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the sample view */
    view_set_enter_callback(app->sample_view, sample_view_enter_callback);
    view_set_exit_callback(app->sample_view, sample_view_exit_callback);

    /* Set the context for the sample view callbacks */
    view_set_context(app->sample_view, app);

    /* Allocate space for the sample view model */
    view_allocate_model(app->sample_view, ViewModelTypeLockFree, sizeof(SampleModel));

    /* Point the LRF sample ring buffer to the shared storage area */
    SampleModel* sample_model = view_get_model(app->sample_view);
    sample_model->samples = (LRFSample*)app->shared_storage;
    sample_model->max_samples = sizeof(app->shared_storage) / sizeof(LRFSample);
    FURI_LOG_D(TAG, "Sampler ring buffer size: %d samples", sample_model->max_samples);

    /* Add the sample view */
    view_dispatcher_add_view(app->view_dispatcher, view_sample, app->sample_view);

    /* Setup the LRF info view */

    /* Allocate space for the LRF info view */
    app->lrfinfo_view = view_alloc();

    /* Setup the draw callback for the LRF info view */
    view_set_draw_callback(app->lrfinfo_view, lrfinfo_view_draw_callback);

    /* Setup the input callback for the LRF info view */
    view_set_input_callback(app->lrfinfo_view, lrfinfo_view_input_callback);

    /* Configure the "previous" callback for the LRF info view */
    view_set_previous_callback(app->lrfinfo_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the LRF info view */
    view_set_enter_callback(app->lrfinfo_view, lrfinfo_view_enter_callback);
    view_set_exit_callback(app->lrfinfo_view, lrfinfo_view_exit_callback);

    /* Set the context for the LRF info view callbacks */
    view_set_context(app->lrfinfo_view, app);

    /* Allocate space for the LRF info view model */
    view_allocate_model(app->lrfinfo_view, ViewModelTypeLockFree, sizeof(LRFInfoModel));

    /* Add the LRF info view */
    view_dispatcher_add_view(app->view_dispatcher, view_lrfinfo, app->lrfinfo_view);

    /* Setup the save diagnostic view */

    /* Allocate space for the save diagnostic view */
    app->savediag_view = view_alloc();

    /* Setup the draw callback for the save diagnostic view */
    view_set_draw_callback(app->savediag_view, savediag_view_draw_callback);

    /* Setup the input callback for the save diagnostic view */
    view_set_input_callback(app->savediag_view, savediag_view_input_callback);

    /* Configure the "previous" callback for the save diagnostic view */
    view_set_previous_callback(app->savediag_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the save diagnostic view */
    view_set_enter_callback(app->savediag_view, savediag_view_enter_callback);
    view_set_exit_callback(app->savediag_view, savediag_view_exit_callback);

    /* Set the context for the save diagnostic view callbacks */
    view_set_context(app->savediag_view, app);

    /* Allocate space for the save diagnostic view model */
    view_allocate_model(app->savediag_view, ViewModelTypeLockFree, sizeof(SaveDiagModel));

    /* Add the save diagnostic view */
    view_dispatcher_add_view(app->view_dispatcher, view_savediag, app->savediag_view);

    /* Setup the test laser view */

    /* Allocate space for the test laser view */
    app->testlaser_view = view_alloc();

    /* Setup the draw callback for the test laser view */
    view_set_draw_callback(app->testlaser_view, testlaser_view_draw_callback);

    /* Configure the "previous" callback for the test laser view */
    view_set_previous_callback(app->testlaser_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the test laser view */
    view_set_enter_callback(app->testlaser_view, testlaser_view_enter_callback);
    view_set_exit_callback(app->testlaser_view, testlaser_view_exit_callback);

    /* Set the context for the test laser view callbacks */
    view_set_context(app->testlaser_view, app);

    /* Allocate space for the test laser view model */
    view_allocate_model(app->testlaser_view, ViewModelTypeLockFree, sizeof(TestLaserModel));

    /* Add the test laser view */
    view_dispatcher_add_view(app->view_dispatcher, view_testlaser, app->testlaser_view);

    /* Setup the test pointer view */

    /* Allocate space for the test pointer view */
    app->testpointer_view = view_alloc();

    /* Setup the draw callback for the test pointer view */
    view_set_draw_callback(app->testpointer_view, testpointer_view_draw_callback);

    /* Configure the "previous" callback for the test pointer view */
    view_set_previous_callback(app->testpointer_view, return_to_submenu_callback);

    /* Configure the enter and exit callbacks for the test pointer view */
    view_set_enter_callback(app->testpointer_view, testpointer_view_enter_callback);
    view_set_exit_callback(app->testpointer_view, testpointer_view_exit_callback);

    /* Set the context for the test pointer view callbacks */
    view_set_context(app->testpointer_view, app);

    /* Allocate space for the test pointer view model */
    view_allocate_model(app->testpointer_view, ViewModelTypeLockFree, sizeof(TestPointerModel));

    /* Add the test pointer view */
    view_dispatcher_add_view(app->view_dispatcher, view_testpointer, app->testpointer_view);

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

    /* Set the default SMM prefix configuration option values (i.e. none) */
    app->smm_pfx_config.config_smm_pfx_label[0] = 0;
    app->smm_pfx_config.config_smm_pfx_names[0][0] = 0;
    app->smm_pfx_config.config_smm_pfx_names[0][1] = 0;

    /* Set the default sampling mode setting */
    app->config.mode = config_mode_values[0];
    variable_item_set_current_value_index(app->item_mode, 0);
    variable_item_set_current_value_text(app->item_mode, config_mode_names[0]);

    /* Set the default buffering setting */
    app->config.buf = config_buf_values[0];
    variable_item_set_current_value_index(app->item_buf, 0);
    variable_item_set_current_value_text(app->item_buf, config_buf_names[0]);

    /* Set the default beep option */
    app->config.beep = config_beep_values[0];
    variable_item_set_current_value_index(app->item_beep, 0);
    variable_item_set_current_value_text(app->item_beep, config_beep_names[0]);

    /* Set the default baudrate option */
    app->config.baudrate = config_baudrate_values[0];
    variable_item_set_current_value_index(app->item_baudrate, 0);
    variable_item_set_current_value_text(app->item_baudrate, config_baudrate_names[0]);

    /* Set the default USB passthrough channel option */
    app->config.passthru_chan = config_passthru_chan_values[0];
    variable_item_set_current_value_index(app->item_passthru_chan, 0);
    variable_item_set_current_value_text(app->item_passthru_chan, config_passthru_chan_names[0]);

    /* Set the default SMM prefix option */
    app->config.smm_pfx = config_smm_pfx_values[0];

    /* Set the default submenu item */
    app->config.sitem = submenu_config;

    /* Assume the pointer is off */
    app->pointer_is_on = false;

    /* Try to load the configuration file and restore the configuration from the
     saved values */
    load_configuration(app);

    /* Setup the backlight control */
    set_backlight_control(&app->backlight_control);

    /* Setup the speaker control */
    set_speaker_control(&app->speaker_control);

    /* Initialize the LRF serial communication app */
    app->lrf_serial_comm_app = lrf_serial_comm_app_init(
        min_led_flash_duration, uart_rx_timeout, app->shared_storage, sizeof(app->shared_storage));

    return app;
}

/** Free up the space allocated for the app **/
static void noptel_lrf_sampler_app_free(App* app) {
    FURI_LOG_I(TAG, "App free");

    /* Stop and free up the LRF serial communication app */
    lrf_serial_comm_app_free(app->lrf_serial_comm_app);

    /* Release the speaker control */
    release_speaker_control(&app->speaker_control);

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

    /* Remove the test pointer view */
    view_dispatcher_remove_view(app->view_dispatcher, view_testpointer);
    view_free(app->testpointer_view);

    /* Remove the test laser view */
    view_dispatcher_remove_view(app->view_dispatcher, view_testlaser);
    view_free(app->testlaser_view);

    /* Remove the save diagnostic view */
    view_dispatcher_remove_view(app->view_dispatcher, view_savediag);
    view_free(app->savediag_view);

    /* Remove the LRF info view */
    view_dispatcher_remove_view(app->view_dispatcher, view_lrfinfo);
    view_free(app->lrfinfo_view);

    /* Remove the sample view */
    view_dispatcher_remove_view(app->view_dispatcher, view_sample);
    view_free(app->sample_view);

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
int32_t noptel_lrf_sampler_app_entry(void* p) {
    UNUSED(p);

    /* Turn on the LRF */
    FURI_LOG_I(TAG, "LRF power on");
    power_lrf(true);

    /* Initialize the app */
    App* app = noptel_lrf_sampler_app_init();

    /* Run the view dispatcher */
    FURI_LOG_D(TAG, "Run view dispatcher");
    view_dispatcher_run(app->view_dispatcher);

    /* Free up the space for the app */
    noptel_lrf_sampler_app_free(app);

    /* Turn off the LRF */
    FURI_LOG_I(TAG, "LRF power off");
    power_lrf(false);

    return 0;
}
