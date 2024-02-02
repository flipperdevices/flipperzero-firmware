#include "infrared_last_settings.h"

#include <furi_hal_infrared.h>

#define TAG "InfraredLastSettings"

#define INFRARED_LAST_SETTINGS_FILE_TYPE "Flipper Infrared Last Settings File"
#define INFRARED_LAST_SETTINGS_FILE_VERSION 1
#define INFRARED_LAST_SETTINGS_PATH EXT_PATH("infrared/assets/last_infrared.settings")

#define INFRARED_LAST_SETTINGS_FIELD_EXTPOWER "External5V"
#define INFRARED_LAST_SETTINGS_FIELD_EXTOUT "ExternalOut"
#define INFRARED_LAST_SETTINGS_FIELD_AUTO_DETECT "AutoDetect"

InfraredLastSettings* infrared_last_settings_alloc(void) {
    InfraredLastSettings* instance = malloc(sizeof(InfraredLastSettings));
    return instance;
}

void infrared_last_settings_free(InfraredLastSettings* instance) {
    furi_assert(instance);
    free(instance);
}

void infrared_last_settings_load(InfraredLastSettings* instance) {
    furi_assert(instance);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* fff_data_file = flipper_format_file_alloc(storage);

    bool temp_extpower = false;
    bool temp_extout = false;
    bool temp_auto_detect = true;

    if(FSE_OK == storage_sd_status(storage) && INFRARED_LAST_SETTINGS_PATH &&
       flipper_format_file_open_existing(fff_data_file, INFRARED_LAST_SETTINGS_PATH)) {
        flipper_format_read_bool(
            fff_data_file, INFRARED_LAST_SETTINGS_FIELD_EXTPOWER, (bool*)&temp_extpower, 1);
        flipper_format_read_bool(
            fff_data_file, INFRARED_LAST_SETTINGS_FIELD_EXTOUT, (bool*)&temp_extout, 1);
        flipper_format_read_bool(
            fff_data_file, INFRARED_LAST_SETTINGS_FIELD_AUTO_DETECT, (bool*)&temp_auto_detect, 1);
    } else {
        FURI_LOG_E(TAG, "Error open file %s", INFRARED_LAST_SETTINGS_PATH);
    }

    instance->ext_5v = temp_extpower;
    instance->ext_out = temp_extout;
    instance->auto_detect = temp_auto_detect;

    flipper_format_file_close(fff_data_file);
    flipper_format_free(fff_data_file);
    furi_record_close(RECORD_STORAGE);
}

bool infrared_last_settings_save(InfraredLastSettings* instance) {
    furi_assert(instance);

    bool saved = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    do {
        if(FSE_OK != storage_sd_status(storage)) {
            break;
        }

        // Open file
        if(!flipper_format_file_open_always(file, INFRARED_LAST_SETTINGS_PATH)) break;

        // Write header
        if(!flipper_format_write_header_cstr(
               file, INFRARED_LAST_SETTINGS_FILE_TYPE, INFRARED_LAST_SETTINGS_FILE_VERSION))
            break;

        if(!flipper_format_insert_or_update_bool(
               file, INFRARED_LAST_SETTINGS_FIELD_EXTPOWER, &instance->ext_5v, 1))
            break;
        if(!flipper_format_insert_or_update_bool(
               file, INFRARED_LAST_SETTINGS_FIELD_EXTOUT, &instance->ext_out, 1))
            break;
        if(!flipper_format_insert_or_update_bool(
               file, INFRARED_LAST_SETTINGS_FIELD_AUTO_DETECT, &instance->auto_detect, 1))
            break;

        saved = true;
    } while(0);

    if(!saved) {
        FURI_LOG_E(TAG, "Error save file %s", INFRARED_LAST_SETTINGS_PATH);
    }

    flipper_format_file_close(file);
    flipper_format_free(file);
    furi_record_close(RECORD_STORAGE);

    return saved;
}

void infrared_last_settings_apply(InfraredLastSettings* instance) {
    furi_assert(instance);

    instance->_otg_was_enabled = furi_hal_power_is_otg_enabled();
    furi_hal_infrared_set_auto_detect(instance->auto_detect);
    if(!instance->auto_detect) {
        furi_hal_infrared_set_debug_out(instance->ext_out);
        if(instance->ext_5v) {
            uint8_t attempts = 0;
            while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
                furi_hal_power_enable_otg();
                furi_delay_ms(10);
            }
        } else if(furi_hal_power_is_otg_enabled()) {
            furi_hal_power_disable_otg();
        }
    } else if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }
}

void infrared_last_settings_reset(InfraredLastSettings* instance) {
    furi_assert(instance);

    if(instance->_otg_was_enabled != furi_hal_power_is_otg_enabled()) {
        if(instance->_otg_was_enabled) {
            uint8_t attempts = 0;
            while(!furi_hal_power_is_otg_enabled() && attempts++ < 5) {
                furi_hal_power_enable_otg();
                furi_delay_ms(10);
            }
        } else {
            furi_hal_power_disable_otg();
        }
    }
}