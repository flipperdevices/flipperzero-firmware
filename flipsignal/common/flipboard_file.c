#include "flipboard_file_i.h"

/**
 * @brief    Check if a directory exists, create it if it doesn't.
 * @param    storage    The storage to use.
 * @param    dir        The directory to check.
*/
static void ensure_dir_exists(Storage* storage, char* dir) {
    if(!storage_dir_exists(storage, dir)) {
        FURI_LOG_I(TAG, "Creating directory: %s", dir);
        storage_simply_mkdir(storage, dir);
    }
}

/**
 * @brief   Check if the save folder exists, create it if it doesn't.
 * @param   storage    The storage to use.
*/
static void ensure_save_folder_exists(Storage* storage) {
    ensure_dir_exists(storage, FLIPBOARD_APPS_DATA_FOLDER);
    ensure_dir_exists(storage, FLIPBOARD_SAVE_FOLDER);
}

/**
 * @brief    Save the flipboard model to the settings file.
 * @param    model    The flipboard model to save.
 * @param    fields   The fields to save.
*/
bool flipboard_model_save(FlipboardModel* model, ActionModelFields fields) {
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
            FURI_LOG_E(TAG, "Failed to open storage");
            break;
        }

        ensure_save_folder_exists(storage);

        if(!flipper_format_file_open_always(format, furi_string_get_cstr(file_path))) {
            FURI_LOG_E(
                TAG, "Failed to open file for writing: \"%s\"", furi_string_get_cstr(file_path));
            break;
        }

        if(!flipper_format_write_header_cstr(format, FLIPBOARD_HEADER, FLIPBOARD_VERSION)) {
            FURI_LOG_E(TAG, "Failed to write header");
            break;
        }

        if(!flipper_format_write_comment_cstr(format, BUY_MSG " " FLIPBOARD_URL)) {
            FURI_LOG_E(TAG, "Failed to write comment");
            break;
        }

        for(int i = 0; i < 16; i++) {
            if(flipboard_model_get_action_model(model, i) != NULL) {
                action_model_save(flipboard_model_get_action_model(model, i), format, fields);
            }
        }

        success = true;
    } while(false);

    flipper_format_free(format);
    furi_string_free(buffer);
    furi_string_free(file_path);
    furi_record_close(RECORD_STORAGE);
    return success;
}

/**
 * @brief    Load the flipboard model from the settings file.
 * @param    model    The flipboard model to populate.
*/
bool flipboard_model_load(FlipboardModel* model) {
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
        flipboard_model_set_action_model(model, i, NULL);
    }

    do {
        uint32_t version = 0;

        if(!storage) {
            FURI_LOG_E(TAG, "Failed to open storage");
            break;
        }

        if(!flipper_format_file_open_existing(format, furi_string_get_cstr(file_path))) {
            FURI_LOG_E(
                TAG, "Failed to open file for reading: \"%s\"", furi_string_get_cstr(file_path));
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
            flipboard_model_set_action_model(model, i, action_model_alloc_from_ff(i, format));
        }

        success = true;
    } while(false);

    for(size_t i = 1; i < 16;) {
        if(flipboard_model_get_action_model(model, i) == NULL) {
            flipboard_model_set_action_model(model, i, action_model_alloc(i));
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