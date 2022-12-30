#include <lib/toolbox/random_name.h>
#include "../mag_i.h"

void mag_scene_save_name_on_enter(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;
    FuriString* folder_path;
    folder_path = furi_string_alloc();

    bool key_name_is_empty = furi_string_empty(mag->file_name);

    if(key_name_is_empty) {
        furi_string_set(mag->file_path, MAG_APP_FOLDER);
        set_random_name(mag->text_store, MAG_TEXT_STORE_SIZE);
        furi_string_set(folder_path, MAG_APP_FOLDER);
    } else {
        mag_text_store_set(mag, "%s", furi_string_get_cstr(mag->file_name));
        path_extract_dirname(furi_string_get_cstr(mag->file_path), folder_path);
    }

    text_input_set_header_text(text_input, "Name the card");
    text_input_set_result_callback(
        text_input,
        mag_text_input_callback,
        mag,
        mag->text_store,
        MAG_DEV_NAME_MAX_LEN,
        key_name_is_empty);

    FURI_LOG_I("", "%s %s", furi_string_get_cstr(folder_path), mag->text_store);

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path),
        MAG_APP_EXTENSION,
        furi_string_get_cstr(mag->file_name));
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    furi_string_free(folder_path);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewTextInput);
}

bool mag_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagEventNext) {
            consumed = true;
            if(!furi_string_empty(mag->file_name)) {
                mag_delete_key(mag);
            }

            furi_string_set(mag->file_name, mag->text_store);

            if(mag_save_key(mag)) {
                scene_manager_next_scene(scene_manager, MagSceneSaveSuccess);
                if(scene_manager_has_previous_scene(scene_manager, MagSceneSavedMenu)) {
                    // Nothing, do not count editing as saving
                    //} else if(scene_manager_has_previous_scene(scene_manager, MagSceneSaveType)) {
                    //DOLPHIN_DEED(DolphinDeedRfidAdd);
                    // TODO: replace dolphin deed!
                } else {
                    //DOLPHIN_DEED(DolphinDeedRfidSave);
                    // TODO: replace dolphin deed!
                }
            } else {
                //scene_manager_search_and_switch_to_previous_scene(
                //    scene_manager, MagSceneReadKeyMenu);
                // TODO: Replace with appropriate scene! No read scene prior if adding manually...
            }
        }
    }

    return consumed;
}

void mag_scene_save_name_on_exit(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;

    void* validator_context = text_input_get_validator_callback_context(text_input);
    text_input_set_validator(text_input, NULL, NULL);
    validator_is_file_free((ValidatorIsFile*)validator_context);

    text_input_reset(text_input);
}
