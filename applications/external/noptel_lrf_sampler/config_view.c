/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.9
 *
 * Configuration view
***/

/*** Includes ***/
#include "common.h"
#include "config_view.h"

/*** Routines ***/

/** Sampling mode setting change function **/
void config_mode_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new sampling mode setting item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new sampling mode setting */
    app->config.mode = config_mode_values[idx];
    variable_item_set_current_value_text(item, config_mode_names[idx]);

    FURI_LOG_D(TAG, "Sampling mode setting change: %s", config_mode_names[idx]);
}

/** Buffering setting change function **/
void config_buf_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new buffering setting item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new buffering setting */
    app->config.buf = config_buf_values[idx];
    variable_item_set_current_value_text(item, config_buf_names[idx]);

    FURI_LOG_D(TAG, "Buffering setting change: %s", config_buf_names[idx]);
}

/** Beep option change function **/
void config_beep_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new beep option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new beep option */
    app->config.beep = config_beep_values[idx];
    variable_item_set_current_value_text(item, config_beep_names[idx]);

    FURI_LOG_D(TAG, "Beep option change: %s", config_beep_names[idx]);
}

/** Baudrate option change function **/
void config_baudrate_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new baudrate option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new baudrate option */
    app->config.baudrate = config_baudrate_values[idx];
    variable_item_set_current_value_text(item, config_baudrate_names[idx]);

    FURI_LOG_D(TAG, "Baudrate option change: %s", config_baudrate_names[idx]);
}

/** USB passthrough channel option change function **/
void config_passthru_chan_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new USB passthrough_channel option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new USB passthrough channel option */
    app->config.passthru_chan = config_passthru_chan_values[idx];
    variable_item_set_current_value_text(item, config_passthru_chan_names[idx]);

    FURI_LOG_D(TAG, "USB passthrough channel option change: %s", config_passthru_chan_names[idx]);
}

/** SMM prefix option change function **/
void config_smm_pfx_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new SMM prefix option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new SMM prefix option */
    app->config.smm_pfx = config_smm_pfx_values[idx];
    variable_item_set_current_value_text(item, app->smm_pfx_config.config_smm_pfx_names[idx]);

    FURI_LOG_D(TAG, "SMM prefix option change: %s", app->smm_pfx_config.config_smm_pfx_names[idx]);
}
