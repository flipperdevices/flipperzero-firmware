#include "flipboard_file.h"
#include <storage/storage.h>
#include <flipper_format.h>

#define TAG "FlipboardFile"
#define FLIPBOARD_KEY_NAME_SIZE 25
#define FLIPBOARD_APPS_DATA_FOLDER EXT_PATH("apps_data")
#define FLIPBOARD_SAVE_FOLDER      \
    FLIPBOARD_APPS_DATA_FOLDER "/" \
                               "flipboard"
#define FLIPBOARD_SAVE_EXTENSION ".txt"

#define FLIPBOARD_HEADER "Flipper Flipboard File"
#define FLIPBOARD_VERSION 1

void ensure_dir_exists(Storage* storage) {
    // If apps_data directory doesn't exist, create it.
    if(!storage_dir_exists(storage, FLIPBOARD_APPS_DATA_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", FLIPBOARD_APPS_DATA_FOLDER);
        storage_simply_mkdir(storage, FLIPBOARD_APPS_DATA_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", FLIPBOARD_APPS_DATA_FOLDER);
    }

    // If wiegand directory doesn't exist, create it.
    if(!storage_dir_exists(storage, FLIPBOARD_SAVE_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", FLIPBOARD_SAVE_FOLDER);
        storage_simply_mkdir(storage, FLIPBOARD_SAVE_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", FLIPBOARD_SAVE_FOLDER);
    }
}

bool flipboard_save(FlipboardModel* model) {
    bool success = false;
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FuriString* file_path = furi_string_alloc();
    FuriString* buffer = furi_string_alloc();
    furi_string_printf(
        file_path, "%s/%s%s", FLIPBOARD_SAVE_FOLDER, "flipboard", FLIPBOARD_SAVE_EXTENSION);
    ensure_dir_exists(storage);
    FlipperFormat* format = flipper_format_file_alloc(storage);

    do {
        if(!storage) {
            printf("Failed to open storage\r\n");
            break;
        }

        if(!flipper_format_file_open_always(format, furi_string_get_cstr(file_path))) {
            printf("Failed to open file for writing: \"%s\"\r\n", furi_string_get_cstr(file_path));
            break;
        }
        if(!flipper_format_write_header_cstr(format, FLIPBOARD_HEADER, FLIPBOARD_VERSION)) break;
        if(!flipper_format_write_comment_cstr(format, "Just test file")) break;
        for(int i = 0; i < 10; i++) {
            furi_string_cat_printf(
                buffer,
                "%d %d ",
                model->setting_key1_key_index[i],
                model->setting_key1_count_index[i]);
        }
        if(!flipper_format_write_string_cstr(format, "Key1", furi_string_get_cstr(buffer))) break;

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
        file_path, "%s/%s%s", FLIPBOARD_SAVE_FOLDER, "flipboard", FLIPBOARD_SAVE_EXTENSION);
    FlipperFormat* format = flipper_format_file_alloc(storage);

    for(size_t i = 0; i < COUNT_OF(model->setting_key1_key_index); i++) {
        model->setting_key1_key_index[i] = 0;
        model->setting_key1_count_index[i] = 0;
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

        if(flipper_format_read_string(format, "Key1", buffer)) {
            size_t begin = 0;
            for(size_t i = 0; i < COUNT_OF(model->setting_key1_key_index); i++) {
                size_t delim = furi_string_search_char(buffer, ' ', begin);
                if(delim != (size_t)-1) {
                    sscanf(
                        furi_string_get_cstr(buffer) + begin,
                        "%hhd",
                        &model->setting_key1_key_index[i]);
                    begin = delim + 1;

                    delim = furi_string_search_char(buffer, ' ', begin);
                    if(delim == (size_t)-1) {
                        delim = furi_string_size(buffer) - 1;
                    }
                    if(delim != (size_t)-1) {
                        sscanf(
                            furi_string_get_cstr(buffer) + begin,
                            "%hhd",
                            &model->setting_key1_count_index[i]);
                        begin = delim + 1;
                    } else {
                        model->setting_key1_count_index[i] = 0;
                    }
                } else {
                    model->setting_key1_key_index[i] = 0;
                    model->setting_key1_count_index[i] = 0;
                }
            }
        }

        success = true;
    } while(false);

    FURI_LOG_D(TAG, "Load %s.", success ? "successful" : "failed");

    flipper_format_free(format);
    furi_string_free(file_path);
    furi_string_free(buffer);
    furi_record_close(RECORD_STORAGE);

    return success;
}