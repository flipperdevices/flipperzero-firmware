/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Configuration view
***/

/*** Includes ***/
#include "common.h"
#include "config_view.h"

/*** Routines ***/

/** Channel option change function **/
void config_channel_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new channel option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new channel option */
    app->config.channel = config_channel_values[idx];
    variable_item_set_current_value_text(item, config_channel_names[idx]);

    FURI_LOG_D(TAG, "Channel option change: %s", config_channel_names[idx]);
}

/** Address option change function **/
void config_address_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new address option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new address option */
    app->config.address = config_address_values[idx];
    variable_item_set_current_value_text(item, config_address_names[idx]);

    FURI_LOG_D(TAG, "Address option change: %s", config_address_names[idx]);
}

/** TX power option change function **/
void config_txpower_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new TX power option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new TX power option */
    app->config.txpower = config_txpower_values[idx];
    variable_item_set_current_value_text(item, config_txpower_names[idx]);

    FURI_LOG_D(TAG, "TX power option change: %s", config_txpower_names[idx]);
}

/** Mode option change function **/
void config_mode_change(VariableItem* item) {
    App* app = variable_item_get_context(item);
    uint8_t idx;

    /* Get the new mode option item index */
    idx = variable_item_get_current_value_index(item);

    /* Set the new mode option */
    app->config.mode = config_mode_values[idx];
    variable_item_set_current_value_text(item, config_mode_names[idx]);

    FURI_LOG_D(TAG, "Mode option change: %s", config_mode_names[idx]);
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
