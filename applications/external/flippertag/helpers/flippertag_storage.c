#include "flippertag_storage.h"

static Storage* flippertag_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void flippertag_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

static void flippertag_close_config_file(FlipperFormat* file) {
    if(file == NULL) return;
    flipper_format_file_close(file);
    flipper_format_free(file);
}

void flippertag_save_settings(void* context) {
    FlipperTag* app = context;
    if(app->save_settings == 0) {
        return;
    }

    FURI_LOG_D(TAG, "Saving Settings");
    Storage* storage = flippertag_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    // Overwrite wont work, so delete first
    if(storage_file_exists(storage, FLIPPERTAG_SETTINGS_SAVE_PATH)) {
        storage_simply_remove(storage, FLIPPERTAG_SETTINGS_SAVE_PATH);
    }

    // Open File, create if not exists
    if(!storage_common_stat(storage, FLIPPERTAG_SETTINGS_SAVE_PATH, NULL) == FSE_OK) {
        FURI_LOG_D(
            TAG, "Config file %s is not found. Will create new.", FLIPPERTAG_SETTINGS_SAVE_PATH);
        if(storage_common_stat(storage, CONFIG_FILE_DIRECTORY_PATH, NULL) == FSE_NOT_EXIST) {
            FURI_LOG_D(
                TAG, "Directory %s doesn't exist. Will create new.", CONFIG_FILE_DIRECTORY_PATH);
            if(!storage_simply_mkdir(storage, CONFIG_FILE_DIRECTORY_PATH)) {
                FURI_LOG_E(TAG, "Error creating directory %s", CONFIG_FILE_DIRECTORY_PATH);
            }
        }
    }

    if(!flipper_format_file_open_new(fff_file, FLIPPERTAG_SETTINGS_SAVE_PATH)) {
        //totp_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Error creating new file %s", FLIPPERTAG_SETTINGS_SAVE_PATH);
        flippertag_close_storage();
        return;
    }

    // Store Settings
    flipper_format_write_header_cstr(
        fff_file, FLIPPERTAG_SETTINGS_HEADER, FLIPPERTAG_SETTINGS_FILE_VERSION);
    flipper_format_write_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_write_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_write_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_write_uint32(
        fff_file, FLIPPERTAG_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    if(!flipper_format_rewind(fff_file)) {
        flippertag_close_config_file(fff_file);
        FURI_LOG_E(TAG, "Rewind error");
        flippertag_close_storage();
        return;
    }

    flippertag_close_config_file(fff_file);
    flippertag_close_storage();
}

void flippertag_read_settings(void* context) {
    FlipperTag* app = context;
    Storage* storage = flippertag_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    if(storage_common_stat(storage, FLIPPERTAG_SETTINGS_SAVE_PATH, NULL) != FSE_OK) {
        flippertag_close_config_file(fff_file);
        flippertag_close_storage();
        return;
    }
    uint32_t file_version;
    FuriString* temp_str = furi_string_alloc();

    if(!flipper_format_file_open_existing(fff_file, FLIPPERTAG_SETTINGS_SAVE_PATH)) {
        FURI_LOG_E(TAG, "Cannot open file %s", FLIPPERTAG_SETTINGS_SAVE_PATH);
        flippertag_close_config_file(fff_file);
        flippertag_close_storage();
        return;
    }

    if(!flipper_format_read_header(fff_file, temp_str, &file_version)) {
        FURI_LOG_E(TAG, "Missing Header Data");
        flippertag_close_config_file(fff_file);
        flippertag_close_storage();
        return;
    }

    furi_string_free(temp_str);

    if(file_version < FLIPPERTAG_SETTINGS_FILE_VERSION) {
        FURI_LOG_I(TAG, "old config version, will be removed.");
        flippertag_close_config_file(fff_file);
        flippertag_close_storage();
        return;
    }

    flipper_format_read_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_HAPTIC, &app->haptic, 1);
    flipper_format_read_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_SPEAKER, &app->speaker, 1);
    flipper_format_read_uint32(fff_file, FLIPPERTAG_SETTINGS_KEY_LED, &app->led, 1);
    flipper_format_read_uint32(
        fff_file, FLIPPERTAG_SETTINGS_KEY_SAVE_SETTINGS, &app->save_settings, 1);

    flipper_format_rewind(fff_file);

    flippertag_close_config_file(fff_file);
    flippertag_close_storage();
}
