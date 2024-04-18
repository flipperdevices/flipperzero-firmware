/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.3
 *
 * Configuration saving / restoring
***/

/*** Includes ***/
#include <storage/storage.h>
#include <gui/view.h>

#include "noptel_lrf_sampler.h"

/*** Routines ***/

/** Load saved configuration options
    Silently fail **/
void load_configuration(App* app) {
    SamplerModel* sampler_model = view_get_model(app->sample_view);
    Storage* storage;
    File* file;
    Config read_config;
    bool file_read = false;
    uint16_t bytes_read = 0;
    uint8_t freq_idx, avg_idx, beep_idx;

    /* Open storage and allocate space for the file*/
    storage = furi_record_open(RECORD_STORAGE);
    file = storage_file_alloc(storage);

    /* Attempt to open the file */
    if(storage_file_open(file, config_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        /* Read the file */
        bytes_read = storage_file_read(file, &read_config, sizeof(Config));

        /* Close the file */
        storage_file_close(file);

        file_read = true;
    } else
        FURI_LOG_I(TAG, "Could not read config file %s", config_file);

    /* Free the file and close storage */
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    /* If we couldn't read the file, give up */
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

    /* Check that the frequency exists */
    for(freq_idx = 0;
        freq_idx < nb_config_freq_values && read_config.freq != config_freq_values[freq_idx];
        freq_idx++)
        ;

    if(freq_idx >= nb_config_freq_values) {
        FURI_LOG_I(
            TAG, "Invalid frequency value %d in config file %s", read_config.freq, config_file);
        return;
    }

    /* Check that the averaging time exists */
    for(avg_idx = 0;
        avg_idx < nb_config_avg_values && read_config.avg != config_avg_values[avg_idx];
        avg_idx++)
        ;

    if(avg_idx >= nb_config_avg_values) {
        FURI_LOG_I(
            TAG, "Invalid averaging time value %d in config file %s", read_config.avg, config_file);
        return;
    }

    /* Check that the beep option exists */
    for(beep_idx = 0;
        beep_idx < nb_config_beep_values && read_config.beep != config_beep_values[beep_idx];
        beep_idx++)
        ;

    if(beep_idx >= nb_config_beep_values) {
        FURI_LOG_I(TAG, "Invalid beep option %d in config file %s", read_config.beep, config_file);
        return;
    }

    /* Check that the submenu item selection exists */
    if(read_config.sitem >= total_submenu_items) {
        FURI_LOG_I(
            TAG, "Invalid submenu item %d in config file %s", read_config.sitem, config_file);
        return;
    }

    /* Configure the frequency from the read value */
    sampler_model->config.freq = read_config.freq;
    variable_item_set_current_value_index(app->item_freq, freq_idx);
    variable_item_set_current_value_text(app->item_freq, config_freq_names[freq_idx]);

    /* Configure the averaging time from the read value */
    sampler_model->config.avg = read_config.avg;
    variable_item_set_current_value_index(app->item_avg, avg_idx);
    variable_item_set_current_value_text(app->item_avg, config_avg_names[avg_idx]);

    /* Configure the beep option from the read value */
    sampler_model->config.beep = read_config.beep;
    variable_item_set_current_value_index(app->item_beep, beep_idx);
    variable_item_set_current_value_text(app->item_beep, config_beep_names[beep_idx]);

    /* Restore the saved last selected submenu item */
    sampler_model->config.sitem = read_config.sitem;
    submenu_set_selected_item(app->submenu, read_config.sitem);

    FURI_LOG_I(
        TAG,
        "Restored config frequency %s, averaging time %s, beep %s, "
        "selected submenu item %d",
        config_freq_names[freq_idx],
        config_avg_names[avg_idx],
        config_beep_names[beep_idx],
        read_config.sitem);
}

/** Save configuration options
    Silently fail **/
void save_configuration(App* app) {
    SamplerModel* sampler_model = view_get_model(app->sample_view);
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
        bytes_written = storage_file_write(file, &sampler_model->config, sizeof(Config));
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
