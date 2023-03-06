#include <storage/storage.h>
#include "boilerplate_storage.h"
#include <flipper_format/flipper_format_i.h>
#include "../boilerplate.h"

static Storage* boilerplate_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void boilerplate_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

static void boilerplate_close_config_file(FlipperFormat* file) {
    if (file == NULL) return;
    flipper_format_file_close(file);
    flipper_format_free(file);
}

void boilerplate_save_settings(void* context) {
    Boilerplate* app = context;
    if (app->save_settings == 0) {
        return;
    }

    FURI_LOG_D(TAG, "Saving Settings");
    Storage* storage = boilerplate_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);
    
    // Overwrite wont work, so delete first
    if(storage_file_exists(storage, BOILERPLATE_SETTINGS_SAVE_PATH)) {
        storage_simply_remove(storage, BOILERPLATE_SETTINGS_SAVE_PATH);
    }

    // Open File, create if not exists
    if(!storage_common_stat(storage, BOILERPLATE_SETTINGS_SAVE_PATH, NULL) == FSE_OK) {
        FURI_LOG_D(TAG, "Config file %s is not found. Will create new.", BOILERPLATE_SETTINGS_SAVE_PATH);
        if(storage_common_stat(storage, CONFIG_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG,
                "Directory %s doesn't exist. Will create new.",
                CONFIG_FILE_DIRECTORY_PATH);
            if(!storage_simply_mkdir(storage, CONFIG_FILE_DIRECTORY_PATH)) {
                FURI_LOG_E(TAG, "Error creating directory %s", CONFIG_FILE_DIRECTORY_PATH);
            }
        }
    }

    if(!flipper_format_file_open_new(fff_file, BOILERPLATE_SETTINGS_SAVE_PATH)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error creating new file %s", BOILERPLATE_SETTINGS_SAVE_PATH);
        boilerplate_close_storage();
        return;
    }
    
    // Store Settings
    flipper_format_write_header_cstr(
        fff_file, BOILERPLATE_SETTINGS_HEADER, BOILERPLATE_SETTINGS_FILE_VERSION);
    flipper_format_write_uint32(
        fff_file, BOILERPLATE_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_write_uint32(
        fff_file, BOILERPLATE_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_write_uint32(
        fff_file, BOILERPLATE_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_write_uint32(
        fff_file, BOILERPLATE_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);
    
    if(!flipper_format_rewind(fff_file)) {
        boilerplate_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Rewind error");
        boilerplate_close_storage();
        return;
    }

    boilerplate_close_config_file(fff_file);
    boilerplate_close_storage();
}
/*
void boilerplate_read_settings(void* context) {

}*/