/***
 * Noptel LRF rangefinder sampler for the Flipper Zero
 * Version: 1.6
 *
 * Configuration saving / restoring
***/

/*** Includes ***/
#include <storage/storage.h>
#include <gui/view.h>

#include "noptel_lrf_sampler.h"
#include "config_view.h"

/*** Routines ***/

/** Load saved configuration options
    Silently fail **/
void load_configuration(App* app) {
    SamplerModel* sampler_model = view_get_model(app->sample_view);
    LRFInfoModel* lrfinfo_model = view_get_model(app->lrfinfo_view);
    SaveDiagModel* savediag_model = view_get_model(app->savediag_view);
    TestLaserModel* testlaser_model = view_get_model(app->testlaser_view);
    TestPointerModel* testpointer_model = view_get_model(app->testpointer_view);
    Storage* storage;
    File* file;
    Config read_config;
    SMMPfxConfig read_smm_pfx_config;
    bool file_read;
    uint16_t bytes_read = 0;
    uint8_t mode_idx, buf_idx, beep_idx, baudrate_idx, smm_pfx_idx;
    uint8_t i;

    /* Open storage */
    storage = furi_record_open(RECORD_STORAGE);

    /* Allocate space for the SMM prefix configuration definition file */
    file = storage_file_alloc(storage);

    /* Attempt to open the SMM prefix configuration definition file */
    file_read = false;
    if(storage_file_open(file, smm_pfx_config_definition_file, FSAM_READ, FSOM_OPEN_EXISTING)) {
        /* Read the file */
        bytes_read = storage_file_read(file, &read_smm_pfx_config, sizeof(SMMPfxConfig));

        /* Close the file */
        storage_file_close(file);

        file_read = true;
    }

    /* Free the file */
    storage_file_free(file);

    /* Could we read the file? */
    if(file_read) {
        /* Did we read enough bytes? */
        if(file_read && bytes_read == sizeof(SMMPfxConfig)) {
            /* Do we have a valid SMM prefix configuration label? */
            for(i = 0; i < sizeof(read_smm_pfx_config.config_smm_pfx_label) &&
                       read_smm_pfx_config.config_smm_pfx_label[i] >= 32 &&
                       read_smm_pfx_config.config_smm_pfx_label[i] < 127;
                i++)
                ;

            if(i > 0 && !read_smm_pfx_config.config_smm_pfx_label[i]) {
                /* Do we have a valid first SMM prefix configuration choice name? */
                for(i = 0; i < sizeof(read_smm_pfx_config.config_smm_pfx_names[0]) &&
                           read_smm_pfx_config.config_smm_pfx_names[0][i] >= 32 &&
                           read_smm_pfx_config.config_smm_pfx_names[0][i] < 127;
                    i++)
                    ;

                if(i > 0 && !read_smm_pfx_config.config_smm_pfx_names[0][i]) {
                    /* Do we have a valid second SMM prefix configuration choice name? */
                    for(i = 0; i < sizeof(read_smm_pfx_config.config_smm_pfx_names[1]) &&
                               read_smm_pfx_config.config_smm_pfx_names[1][i] >= 32 &&
                               read_smm_pfx_config.config_smm_pfx_names[1][i] < 127;
                        i++)
                        ;

                    if(i > 0 && !read_smm_pfx_config.config_smm_pfx_names[1][i]) {
                        /* Store the SMM prefix configuration option values */
                        memcpy(&app->smm_pfx_config, &read_smm_pfx_config, sizeof(SMMPfxConfig));

                        /* Add the item to the configuration menu */
                        app->item_smm_pfx = variable_item_list_add(
                            app->config_list,
                            app->smm_pfx_config.config_smm_pfx_label,
                            nb_config_smm_pfx_values,
                            config_smm_pfx_change,
                            app);

                        /* Configure the default SMM prefix option */
                        variable_item_set_current_value_index(app->item_smm_pfx, 0);
                        variable_item_set_current_value_text(
                            app->item_smm_pfx, app->smm_pfx_config.config_smm_pfx_names[0]);
                    } else
                        FURI_LOG_I(
                            TAG,
                            "Invalid second SMM prefix config choice name in SMM "
                            "prefix config definition file %s",
                            smm_pfx_config_definition_file);
                } else
                    FURI_LOG_I(
                        TAG,
                        "Invalid first SMM prefix config choice name in SMM "
                        "prefix config definition file %s",
                        smm_pfx_config_definition_file);
            } else
                FURI_LOG_I(
                    TAG,
                    "Invalid SMM prefix config label in SMM "
                    "prefix config definition file %s",
                    smm_pfx_config_definition_file);
        } else
            FURI_LOG_I(
                TAG,
                "Read %d bytes from SMM prefix config definition file %s "
                "but %d expected",
                bytes_read,
                smm_pfx_config_definition_file,
                sizeof(SMMPfxConfig));
    }

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

    /* Check that the sampling mode setting exists */
    for(mode_idx = 0;
        mode_idx < nb_config_mode_values && read_config.mode != config_mode_values[mode_idx];
        mode_idx++)
        ;

    if(mode_idx >= nb_config_mode_values) {
        FURI_LOG_I(
            TAG, "Invalid sampling mode value %d in config file %s", read_config.mode, config_file);
        return;
    }

    /* Check that the buffering setting exists */
    for(buf_idx = 0;
        buf_idx < nb_config_buf_values && read_config.buf != config_buf_values[buf_idx];
        buf_idx++)
        ;

    if(buf_idx >= nb_config_buf_values) {
        FURI_LOG_I(
            TAG, "Invalid buffering value %d in config file %s", read_config.buf, config_file);
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

    /* Check that the baudrate option exists */
    for(baudrate_idx = 0; baudrate_idx < nb_config_baudrate_values &&
                          read_config.baudrate != config_baudrate_values[baudrate_idx];
        baudrate_idx++)
        ;

    if(baudrate_idx >= nb_config_baudrate_values) {
        FURI_LOG_I(
            TAG,
            "Invalid baudrate option %ld in config file %s",
            read_config.baudrate,
            config_file);
        return;
    }

    /* Check that the SMM prefix option exists */
    for(smm_pfx_idx = 0; smm_pfx_idx < nb_config_smm_pfx_values &&
                         read_config.smm_pfx != config_smm_pfx_values[smm_pfx_idx];
        smm_pfx_idx++)
        ;

    if(smm_pfx_idx >= nb_config_smm_pfx_values) {
        FURI_LOG_I(
            TAG,
            "Invalid SMM prefix option %d in config file %s",
            read_config.smm_pfx,
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

    /* Configure the sampling mode setting from the read value */
    sampler_model->config.mode = read_config.mode;
    variable_item_set_current_value_index(app->item_mode, mode_idx);
    variable_item_set_current_value_text(app->item_mode, config_mode_names[mode_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_mode_label, config_mode_names[mode_idx]);

    /* Configure the buffering setting from the read value */
    sampler_model->config.buf = read_config.buf;
    variable_item_set_current_value_index(app->item_buf, buf_idx);
    variable_item_set_current_value_text(app->item_buf, config_buf_names[buf_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_buf_label, config_buf_names[buf_idx]);

    /* Configure the beep option from the read value */
    sampler_model->config.beep = read_config.beep;
    testlaser_model->beep = read_config.beep;
    testpointer_model->beep = read_config.beep;
    variable_item_set_current_value_index(app->item_beep, beep_idx);
    variable_item_set_current_value_text(app->item_beep, config_beep_names[beep_idx]);
    FURI_LOG_I(TAG, "  %s: %s", config_beep_label, config_beep_names[beep_idx]);

    /* Configure the baudrate option from the read value */
    sampler_model->config.baudrate = read_config.baudrate;
    lrfinfo_model->baudrate = read_config.baudrate;
    savediag_model->baudrate = read_config.baudrate;
    testlaser_model->baudrate = read_config.baudrate;
    testpointer_model->baudrate = read_config.baudrate;
    variable_item_set_current_value_index(app->item_baudrate, baudrate_idx);
    variable_item_set_current_value_text(app->item_baudrate, config_baudrate_names[baudrate_idx]);
    FURI_LOG_I(TAG, "  %s: %s bps", config_baudrate_label, config_baudrate_names[baudrate_idx]);

    /* Restore the saved last selected submenu item */
    sampler_model->config.sitem = read_config.sitem;
    submenu_set_selected_item(app->submenu, read_config.sitem);
    FURI_LOG_I(TAG, "  %s: %s", "Selected submenu item", submenu_item_names[read_config.sitem]);
    furi_delay_ms(500);

    /* Is the SMM prefix configuration option enabled? */
    if(app->smm_pfx_config.config_smm_pfx_label[0]) {
        /* Configure the SMM prefix option from the read value */
        sampler_model->config.smm_pfx = read_config.smm_pfx;
        variable_item_set_current_value_index(app->item_smm_pfx, smm_pfx_idx);
        variable_item_set_current_value_text(
            app->item_smm_pfx, app->smm_pfx_config.config_smm_pfx_names[smm_pfx_idx]);
        FURI_LOG_I(
            TAG,
            "  %s: %s",
            app->smm_pfx_config.config_smm_pfx_label,
            app->smm_pfx_config.config_smm_pfx_names[smm_pfx_idx]);
    }
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
