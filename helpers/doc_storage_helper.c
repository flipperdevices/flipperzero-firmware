#include "doc_storage_helper.h"

#include <doc_icons.h>
#include <furi_hal_power.h>
#include <saved_struct.h>
#include <dialogs/dialogs.h>

bool doc_get_backup_path(Doc* app) {
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, DOC_BACKUP_FILE_EXTENSION, &I_flipperdoc);
    browser_options.base_path = "/data";
    furi_string_set(app->file_path, "/data");

    return dialog_file_browser_show(app->dialogs, app->file_path, app->file_path, &browser_options);
}

bool internal_doc_dolphin_state_load_from_path(DolphinState* dolphin_state, const char* path) {
    DolphinStoreData* data = malloc(sizeof(DolphinStoreData));

    bool success = saved_struct_load(
        path,
        data,
        sizeof(DolphinStoreData),
        DOLPHIN_STATE_HEADER_MAGIC,
        DOLPHIN_STATE_HEADER_VERSION);

    if(success && (data->butthurt > BUTTHURT_MAX || data->butthurt < BUTTHURT_MIN)) {
        success = false;
    }

    dolphin_state->data = *data;
    free(data);

    return success;
}

bool doc_dolphin_state_load_from_path(DolphinState* dolphin_state, const char* path) {
    bool success = internal_doc_dolphin_state_load_from_path(dolphin_state, path);

    if(success) {
        FURI_LOG_I(TAG, "Backup state loaded");
    } else {
        FURI_LOG_E(TAG, "Failed to load backup state");
    }

    return success;
}

bool doc_dolphin_state_load(DolphinState* dolphin_state) {
    bool success = internal_doc_dolphin_state_load_from_path(dolphin_state, DOLPHIN_STATE_PATH);

    if(success) {
        FURI_LOG_I(TAG, "Internal state loaded");
    } else {
        FURI_LOG_W(TAG, "Reset dolphin-state, load error occured");
        memset(dolphin_state, 0, sizeof(*dolphin_state));
        dolphin_state->dirty = true;
    }

    return success;
}

bool doc_internal_dolphin_state_save_to_path(DolphinState* dolphin_state, const char* path) {
    return saved_struct_save(
        path,
        &dolphin_state->data,
        sizeof(DolphinStoreData),
        DOLPHIN_STATE_HEADER_MAGIC,
        DOLPHIN_STATE_HEADER_VERSION);
}

bool doc_dolphin_state_save_to_path(DolphinState* dolphin_state, const char* path, const char* file_name) {
    doc_dolphin_state_load(dolphin_state);

    FuriString* complete_path = furi_string_alloc_set(path);
    furi_string_cat(complete_path, file_name);

    if(!furi_string_end_with(complete_path, DOC_BACKUP_FILE_EXTENSION)) {
        furi_string_cat_str(complete_path, DOC_BACKUP_FILE_EXTENSION);
    }

    bool result = doc_internal_dolphin_state_save_to_path(dolphin_state, furi_string_get_cstr(complete_path));

    if(result) {
        FURI_LOG_I(TAG, "Backup state saved: %s", file_name);
    } else {
        FURI_LOG_E(TAG, "Failed to save backup state: %s", file_name);
    }

    furi_string_free(complete_path);

    return result;
}

bool doc_dolphin_state_save(DolphinState* dolphin_state, bool restart) {
    FURI_LOG_D(TAG, "Should restart: %d", restart);

    bool result = doc_internal_dolphin_state_save_to_path(dolphin_state, DOLPHIN_STATE_PATH);

    if(result) {
        FURI_LOG_I(TAG, "Internal state saved");
        dolphin_state->dirty = false;

        if(restart) {
            furi_delay_ms(50);
            furi_hal_power_reset();
        }
    } else {
        FURI_LOG_E(TAG, "Failed to save internal state");
    }

    return result;
}