#include "quac_settings.h"

#include <flipper_format/flipper_format.h>

// Quac Settings File Info
#define QUAC_SETTINGS_FILE_TYPE "Quac Settings File"
#define QUAC_SETTINGS_FILE_VERSION 1

void quac_set_default_settings(App* app) {
    app->settings.layout = QUAC_APP_LANDSCAPE;
    app->settings.show_icons = true;
    app->settings.show_headers = true;
    app->settings.rfid_duration = 2500;
    app->settings.nfc_duration = 1000;
    app->settings.subghz_repeat = 10;
    app->settings.subghz_use_ext_antenna = false;
    app->settings.show_hidden = false;
}

void quac_load_settings(App* app) {
    FlipperFormat* fff_settings = flipper_format_file_alloc(app->storage);
    FuriString* temp_str;
    temp_str = furi_string_alloc();
    uint32_t temp_data32 = 0;

    // Initialize settings to the defaults
    quac_set_default_settings(app);

    FURI_LOG_I(TAG, "SETTINGS: Reading: %s", QUAC_SETTINGS_PATH);
    do {
        if(!flipper_format_file_open_existing(fff_settings, QUAC_SETTINGS_PATH)) {
            FURI_LOG_I(TAG, "SETTINGS: File not found, using defaults");
            break;
        }

        if(!flipper_format_read_header(fff_settings, temp_str, &temp_data32)) {
            FURI_LOG_E(TAG, "SETTINGS: Missing or incorrect header");
            break;
        }

        if((!strcmp(furi_string_get_cstr(temp_str), QUAC_SETTINGS_FILE_TYPE)) &&
           (temp_data32 == QUAC_SETTINGS_FILE_VERSION)) {
        } else {
            FURI_LOG_E(TAG, "SETTINGS: Type or version mismatch");
            break;
        }

        // Now read actual values we care about
        if(!flipper_format_read_string(fff_settings, "Layout", temp_str)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing Layout");
        } else {
            if(!strcmp(furi_string_get_cstr(temp_str), "Landscape")) {
                app->settings.layout = QUAC_APP_LANDSCAPE;
            } else if(!strcmp(furi_string_get_cstr(temp_str), "Portrait")) {
                app->settings.layout = QUAC_APP_PORTRAIT;
            } else {
                FURI_LOG_E(TAG, "SETTINGS: Invalid Layout");
            }
        }

        if(!flipper_format_read_uint32(fff_settings, "Show Icons", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'Show Icons'");
        } else {
            app->settings.show_icons = (temp_data32 == 0) ? false : true;
        }

        if(!flipper_format_read_uint32(fff_settings, "Show Headers", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'Show Headers'");
        } else {
            app->settings.show_headers = (temp_data32 == 1) ? true : false;
        }

        if(!flipper_format_read_uint32(fff_settings, "RFID Duration", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'RFID Duration'");
        } else {
            app->settings.rfid_duration = temp_data32;
        }

        if(!flipper_format_read_uint32(fff_settings, "NFC Duration", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'NFC Duration'");
        } else {
            app->settings.nfc_duration = temp_data32;
        }

        if(!flipper_format_read_uint32(fff_settings, "SubGHz Repeat", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'SubGHz Repeat'");
        } else {
            app->settings.subghz_repeat = temp_data32;
        }

        if(!flipper_format_read_uint32(fff_settings, "SubGHz Ext Antenna", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'SubGHz Ext Antenna'");
        } else {
            app->settings.subghz_use_ext_antenna = (temp_data32 == 1) ? true : false;
        }

        if(!flipper_format_read_uint32(fff_settings, "Show Hidden", &temp_data32, 1)) {
            FURI_LOG_W(TAG, "SETTINGS: Missing 'Show Hidden'");
        } else {
            app->settings.show_hidden = (temp_data32 == 1) ? true : false;
        }
    } while(false);

    furi_string_free(temp_str);
    flipper_format_free(fff_settings);
}

void quac_save_settings(App* app) {
    FlipperFormat* fff_settings = flipper_format_file_alloc(app->storage);
    uint32_t temp_data32;

    FURI_LOG_I(TAG, "SETTINGS: Saving");
    bool successful = false;
    do {
        if(!flipper_format_file_open_always(fff_settings, QUAC_SETTINGS_PATH)) {
            FURI_LOG_E(TAG, "SETTINGS: Unable to open file for save!!");
            break;
        }

        if(!flipper_format_write_header_cstr(
               fff_settings, QUAC_SETTINGS_FILE_TYPE, QUAC_SETTINGS_FILE_VERSION)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed writing file type and version");
            break;
        }
        // layout, icons, headers, duration
        if(!flipper_format_write_string_cstr(
               fff_settings,
               "Layout",
               app->settings.layout == QUAC_APP_LANDSCAPE ? "Landscape" : "Portrait")) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write Layout");
            break;
        }

        temp_data32 = app->settings.show_icons ? 1 : 0;
        if(!flipper_format_write_uint32(fff_settings, "Show Icons", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'Show Icons'");
            break;
        }
        temp_data32 = app->settings.show_headers ? 1 : 0;
        if(!flipper_format_write_uint32(fff_settings, "Show Headers", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'Show Headers'");
            break;
        }
        if(!flipper_format_write_uint32(
               fff_settings, "RFID Duration", &app->settings.rfid_duration, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'RFID Duration'");
            break;
        }
        if(!flipper_format_write_uint32(
               fff_settings, "NFC Duration", &app->settings.nfc_duration, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'NFC Duration'");
            break;
        }
        if(!flipper_format_write_uint32(
               fff_settings, "SubGHz Repeat", &app->settings.subghz_repeat, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'SubGHz Repeat'");
            break;
        }
        temp_data32 = app->settings.subghz_use_ext_antenna ? 1 : 0;
        if(!flipper_format_write_uint32(fff_settings, "SubGHz Ext Antenna", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'SubGHz Ext Antenna'");
            break;
        }
        temp_data32 = app->settings.show_hidden ? 1 : 0;
        if(!flipper_format_write_uint32(fff_settings, "Show Hidden", &temp_data32, 1)) {
            FURI_LOG_E(TAG, "SETTINGS: Failed to write 'Show Hidden'");
            break;
        }

        successful = true;
    } while(false);

    if(!successful) {
        FURI_LOG_E(TAG, "SETTINGS: Failed to save settings!!");
    }

    flipper_format_file_close(fff_settings);
    flipper_format_free(fff_settings);
}