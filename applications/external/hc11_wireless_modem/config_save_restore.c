/***
 * HC-11 wireless modem emulator for the Flipper Zero
 * Version: 0.1
 *
 * Configuration saving / restoring
***/

/*** Includes ***/
#include <storage/storage.h>
#include <gui/view.h>

#include "common.h"
#include "config_view.h"

/*** Routines ***/

/** Load saved configuration options
    Silently fail **/
void load_configuration(App* app) {
    Storage* storage;
    File* file;
    Config read_config;
    bool file_read;
    uint16_t bytes_read = 0;
    uint8_t channel_idx, address_idx, txpower_idx, mode_idx, passthru_chan_idx;

    /* Open storage */
    storage = furi_record_open(RECORD_STORAGE);

    /* Allocate space for the configuration file */
    file = storage_file_alloc(storage);

    /* Attempt to open the configuration file */
    file_read = false;
    if(storage_file_open(file, config_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        /* Read the file */
        bytes_read = storage_file_read(file, &read_config, sizeof(Config));

        /* Close the file */
        storage_file_close(file);

        file_read = true;
    } else
        FURI_LOG_I(TAG, "Could not read config file %s", config_file);

    /* Free the file */
    storage_file_free(file);

    /* Close storage */
    furi_record_close(RECORD_STORAGE);

    /* If we couldn't read the configuration file, give up */
    if(!file_read) return;

    /* If we didn't read enough bytes, give up */
    if(bytes_read < sizeof(Config)) {
        FURI_LOG_I(
            TAG,
            "Read %d bytes from config file %s but %d expected",
            bytes_read,
            config_file,
            sizeof(Config));
        return;
    }

    /* Check that the channel option exists */
    for(channel_idx = 0; channel_idx < nb_config_channel_values &&
                         read_config.channel != config_channel_values[channel_idx];
        channel_idx++)
        ;

    if(channel_idx >= nb_config_channel_values) {
        FURI_LOG_I(
            TAG, "Invalid channel option %d in config file %s", read_config.channel, config_file);
        return;
    }

    /* Check that the address option exists */
    for(address_idx = 0; address_idx < nb_config_address_values &&
                         read_config.address != config_address_values[address_idx];
        address_idx++)
        ;

    if(address_idx >= nb_config_address_values) {
        FURI_LOG_I(
            TAG, "Invalid address option %d in config file %s", read_config.address, config_file);
        return;
    }

    /* Check that the TX power option exists */
    for(txpower_idx = 0; txpower_idx < nb_config_txpower_values &&
                         read_config.txpower != config_txpower_values[txpower_idx];
        txpower_idx++)
        ;

    if(txpower_idx >= nb_config_txpower_values) {
        FURI_LOG_I(
            TAG, "Invalid TX power option %d in config file %s", read_config.txpower, config_file);
        return;
    }

    /* Check that the mode option exists */
    for(mode_idx = 0;
        mode_idx < nb_config_mode_values && read_config.mode != config_mode_values[mode_idx];
        mode_idx++)
        ;

    if(mode_idx >= nb_config_mode_values) {
        FURI_LOG_I(TAG, "Invalid mode option %d in config file %s", read_config.mode, config_file);
        return;
    }

    /* Check that the USB passthrough channel option exists */
    for(passthru_chan_idx = 0;
        passthru_chan_idx < nb_config_passthru_chan_values &&
        read_config.passthru_chan != config_passthru_chan_values[passthru_chan_idx];
        passthru_chan_idx++)
        ;

    if(passthru_chan_idx >= nb_config_passthru_chan_values) {
        FURI_LOG_I(
            TAG,
            "Invalid USB passthrough channel option %d in config file "
            "%s",
            read_config.passthru_chan,
            config_file);
        return;
    }

    /* Check that the submenu item selection exists */
    if(read_config.sitem >= total_submenu_items) {
        FURI_LOG_I(
            TAG, "Invalid submenu item %d in config file %s", read_config.sitem, config_file);
        return;
    }

    FURI_LOG_I(TAG, "Restored configuration:");

    /* Configure the channel option from the read value */
    app->config.channel = read_config.channel;
    variable_item_set_current_value_index(app->item_channel, channel_idx);
    variable_item_set_current_value_text(app->item_channel, config_channel_names[channel_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_channel_label, config_channel_names[channel_idx]);

    /* Configure the address option from the read value */
    app->config.address = read_config.address;
    variable_item_set_current_value_index(app->item_address, address_idx);
    variable_item_set_current_value_text(app->item_address, config_address_names[address_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_address_label, config_address_names[address_idx]);

    /* Configure the TX power option from the read value */
    app->config.txpower = read_config.txpower;
    variable_item_set_current_value_index(app->item_txpower, txpower_idx);
    variable_item_set_current_value_text(app->item_txpower, config_txpower_names[txpower_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_txpower_label, config_txpower_names[txpower_idx]);

    /* Configure the mode option from the read value */
    app->config.mode = read_config.mode;
    variable_item_set_current_value_index(app->item_mode, mode_idx);
    variable_item_set_current_value_text(app->item_mode, config_mode_names[mode_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_mode_label, config_mode_names[mode_idx]);

    /* Configure the USB passthrough channel option from the read value */
    app->config.passthru_chan = read_config.passthru_chan;
    variable_item_set_current_value_index(app->item_passthru_chan, passthru_chan_idx);
    variable_item_set_current_value_text(
        app->item_passthru_chan, config_passthru_chan_names[passthru_chan_idx]);
    FURI_LOG_I(
        TAG, "  %s: %s", config_passthru_chan_label, config_passthru_chan_names[passthru_chan_idx]);

    /* Restore the saved last selected submenu item */
    app->config.sitem = read_config.sitem;
    submenu_set_selected_item(app->submenu, read_config.sitem);
    FURI_LOG_I(TAG, "  %s: %s", "Selected submenu item", submenu_item_names[read_config.sitem]);
    furi_delay_ms(500);
}

/** Save configuration options
    Silently fail **/
void save_configuration(App* app) {
    Storage* storage;
    File* file;
    bool file_written = false;
    uint16_t bytes_written = 0;

    /* Open storage and allocate space for the file*/
    storage = furi_record_open(RECORD_STORAGE);
    file = storage_file_alloc(storage);

    /* Attempt to open the file */
    if(storage_file_open(file, config_file, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        /* Write the file */
        bytes_written = storage_file_write(file, &app->config, sizeof(Config));
        /* Close the file */
        storage_file_close(file);

        file_written = true;
    } else
        FURI_LOG_I(TAG, "Could not open config file %s for writing", config_file);

    /* Free the file and close storage */
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    /* If we couldn't write the file, stop now */
    if(!file_written) return;

    /* If we didn't write the correct number of bytes, log the error */
    if(bytes_written != sizeof(Config))
        FURI_LOG_I(
            TAG,
            "Wrote %d bytes to config file %s but %d expected",
            bytes_written,
            config_file,
            sizeof(Config));

    FURI_LOG_I(TAG, "Config saved in file %s", config_file);
}
