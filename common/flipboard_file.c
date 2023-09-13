#include "flipboard_file.h"
#include <storage/storage.h>
#include <flipper_format.h>

#include "key_setting_model.h"

#define FLIPBOARD_URL "https://tindie.com/stores/MakeItHackin"

#define FLIPBOARD_KEY_NAME_SIZE 25
#define FLIPBOARD_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define FLIPBOARD_SAVE_FOLDER      \
    FLIPBOARD_APPS_DATA_FOLDER "/" \
                               "flipboard"
#define FLIPBOARD_SAVE_EXTENSION ".txt"

#define FLIPBOARD_HEADER "Flipper Flipboard File"
#define FLIPBOARD_VERSION 1

#define TAG "FlipboardFile"

static void ensure_dir_exists(Storage* storage, char* dir) {
    if(!storage_dir_exists(storage, dir)) {
        FURI_LOG_I(TAG, "Creating directory: %s", dir);
        storage_simply_mkdir(storage, dir);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", dir);
    }
}

static void ensure_save_folder_exists(Storage* storage) {
    ensure_dir_exists(storage, FLIPBOARD_APPS_DATA_FOLDER);
    ensure_dir_exists(storage, FLIPBOARD_SAVE_FOLDER);
}

bool flipboard_save(FlipboardModel* model, KeySettingModelFields fields) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_path = furi_string_alloc();
    FuriString* buffer = furi_string_alloc();

    furi_string_printf(
        file_path,
        "%s/%s%s",
        FLIPBOARD_SAVE_FOLDER,
        flipboard_model_get_name(model),
        FLIPBOARD_SAVE_EXTENSION);

    FlipperFormat* format = flipper_format_file_alloc(storage);

    do {
        if(!storage) {
            printf("Failed to open storage\r\n");
            break;
        }

        ensure_save_folder_exists(storage);

        if(!flipper_format_file_open_always(format, furi_string_get_cstr(file_path))) {
            printf("Failed to open file for writing: \"%s\"\r\n", furi_string_get_cstr(file_path));
            break;
        }

        if(!flipper_format_write_header_cstr(format, FLIPBOARD_HEADER, FLIPBOARD_VERSION)) {
            break;
        }

        if(!flipper_format_write_comment_cstr(format, "Buy your flipboard at " FLIPBOARD_URL)) {
            break;
        }

        for(int i = 0; i < 16; i++) {
            if(flipboard_model_get_key_setting_model(model, i) != NULL) {
                key_setting_model_save(
                    flipboard_model_get_key_setting_model(model, i), format, fields);
            }
        }

        success = true;
    } while(false);

    FURI_LOG_D(TAG, "Save %s.", success ? "successful" : "failed");

    flipper_format_free(format);
    furi_string_free(file_path);
    furi_string_free(buffer);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool flipboard_load(FlipboardModel* model) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_path = furi_string_alloc();
    FuriString* buffer = furi_string_alloc();
    furi_string_printf(
        file_path,
        "%s/%s%s",
        FLIPBOARD_SAVE_FOLDER,
        flipboard_model_get_name(model),
        FLIPBOARD_SAVE_EXTENSION);
    FlipperFormat* format = flipper_format_file_alloc(storage);

    for(size_t i = 0; i < 16; i++) {
        flipboard_model_set_key_setting_model(model, i, NULL);
    }

    do {
        uint32_t version = 0;

        if(!storage) {
            printf("Failed to open storage\r\n");
            break;
        }

        if(!flipper_format_file_open_existing(format, furi_string_get_cstr(file_path))) {
            printf("Failed to open file for reading: \"%s\"\r\n", furi_string_get_cstr(file_path));
            break;
        }
        if(!flipper_format_read_header(format, buffer, &version)) {
            FURI_LOG_E(TAG, "Missing or incorrect header");
            break;
        }
        if(strcmp(furi_string_get_cstr(buffer), FLIPBOARD_HEADER) != 0 ||
           version != FLIPBOARD_VERSION) {
            FURI_LOG_E(TAG, "Type or version mismatch");
            break;
        }
        for(size_t i = 0; i < 16; i++) {
            flipboard_model_set_key_setting_model(
                model, i, key_setting_model_alloc_from_ff(i, format));
        }

        success = true;
    } while(false);

    FURI_LOG_D(TAG, "Load %s.", success ? "successful" : "failed");

    for(size_t i = 1; i < 16;) {
        if(flipboard_model_get_key_setting_model(model, i) == NULL) {
            flipboard_model_set_key_setting_model(model, i, key_setting_model_alloc(i));
        }

        if(flipboard_model_get_single_button_mode(model)) {
            i = i << 1;
        } else {
            i++;
        }
    }

    flipper_format_free(format);
    furi_string_free(file_path);
    furi_string_free(buffer);
    furi_record_close(RECORD_STORAGE);

    return success;
}