#include "../wiegand.h"

void wiegand_data_scene_save_name_text_input_callback(void* context) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, WiegandDataSceneSaveFileEvent);
}

void ensure_dir_exists(Storage* storage) {
    // If apps_data directory doesn't exist, create it.
    if(!storage_dir_exists(storage, WIEGAND_APPS_DATA_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", WIEGAND_APPS_DATA_FOLDER);
        storage_simply_mkdir(storage, WIEGAND_APPS_DATA_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", WIEGAND_APPS_DATA_FOLDER);
    }

    // If wiegand directory doesn't exist, create it.
    if(!storage_dir_exists(storage, WIEGAND_SAVE_FOLDER)) {
        FURI_LOG_I(TAG, "Creating directory: %s", WIEGAND_SAVE_FOLDER);
        storage_simply_mkdir(storage, WIEGAND_SAVE_FOLDER);
    } else {
        FURI_LOG_I(TAG, "Directory exists: %s", WIEGAND_SAVE_FOLDER);
    }
}

void wiegand_save(void* context) {
    App* app = context;
    FuriString* buffer = furi_string_alloc(1024);
    FuriString* file_path = furi_string_alloc();
    furi_string_printf(
        file_path, "%s/%s%s", WIEGAND_SAVE_FOLDER, app->file_name, WIEGAND_SAVE_EXTENSION);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    ensure_dir_exists(storage);
    File* data_file = storage_file_alloc(storage);
    if(storage_file_open(
           data_file, furi_string_get_cstr(file_path), FSAM_WRITE, FSOM_OPEN_ALWAYS)) {
        furi_string_printf(buffer, "Filetype: Flipper Wiegand Key File\n");
        storage_file_write(data_file, furi_string_get_cstr(buffer), furi_string_size(buffer));
        furi_string_printf(buffer, "Version: 1\n");
        storage_file_write(data_file, furi_string_get_cstr(buffer), furi_string_size(buffer));
        furi_string_printf(buffer, "Protocol: RAW\n");
        storage_file_write(data_file, furi_string_get_cstr(buffer), furi_string_size(buffer));
        furi_string_printf(buffer, "Bits: %d\n", bit_count);
        storage_file_write(data_file, furi_string_get_cstr(buffer), furi_string_size(buffer));
        furi_string_printf(buffer, "RAW_Data: ");
        for(int i = 0; i < bit_count; i++) {
            furi_string_cat_printf(
                buffer,
                "D%d %ld %ld ",
                data[i] ? 1 : 0,
                data_fall[i] - data_fall[0],
                data_rise[i] - data_fall[0]);
        }
        furi_string_push_back(buffer, '\n');
        storage_file_write(data_file, furi_string_get_cstr(buffer), furi_string_size(buffer));
        storage_file_close(data_file);
    }

    storage_file_free(data_file);
    furi_record_close(RECORD_STORAGE);
    furi_string_free(file_path);
    furi_string_free(buffer);
}

void wiegand_save_scene_on_enter(void* context) {
    App* app = context;
    text_input_reset(app->text_input);
    set_random_name(app->file_name, WIEGAND_KEY_NAME_SIZE);

    text_input_set_header_text(app->text_input, "Name the key");
    text_input_set_result_callback(
        app->text_input,
        wiegand_data_scene_save_name_text_input_callback,
        app,
        app->file_name,
        WIEGAND_KEY_NAME_SIZE,
        true);

    view_dispatcher_switch_to_view(app->view_dispatcher, WiegandTextInputView);
}

bool wiegand_save_scene_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    switch(event.type) {
    case SceneManagerEventTypeCustom:
        switch(event.event) {
        case WiegandDataSceneSaveFileEvent:
            wiegand_save(app);
            data_saved = true;
            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, WiegandDataScene);
            consumed = true;
            break;
        default:
            consumed = false;
            break;
        }
        break;
    default:
        break;
    }
    return consumed;
}
