#include <toolbox/name_generator.h>
#include "../mag_i.h"

void mag_scene_input_name_on_enter(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;
    FuriString* folder_path;
    folder_path = furi_string_alloc();

    //TODO: compatible types / etc
    //bool name_is_empty = furi_string_empty(mag->mag_dev->dev_name);
    bool name_is_empty = true;

    if(name_is_empty) {
        furi_string_set(mag->file_path, MAG_APP_FOLDER);
        name_generator_make_auto(mag->text_store, MAG_TEXT_STORE_SIZE, "Mag");
        furi_string_set(folder_path, MAG_APP_FOLDER);
    } else {
        // TODO: compatible types etc
        //mag_text_store_set(mag, "%s", furi_string_get_cstr(mag->mag_dev->dev_name));
        path_extract_dirname(furi_string_get_cstr(mag->file_path), folder_path);
    }

    text_input_set_header_text(text_input, "Name the card");
    text_input_set_result_callback(
        text_input,
        mag_text_input_callback,
        mag,
        mag->text_store,
        MAG_DEV_NAME_MAX_LEN,
        name_is_empty);

    FURI_LOG_I("", "%s %s", furi_string_get_cstr(folder_path), mag->text_store);

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path),
        MAG_APP_EXTENSION,
        furi_string_get_cstr(mag->file_name));
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    furi_string_free(folder_path);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewTextInput);
}

bool mag_scene_input_name_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    SceneManager* scene_manager = mag->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == MagEventNext) {
            consumed = true;
            //if(!furi_string_empty(mag->file_name)) {
            //    mag_delete_key(mag);
            //}

            furi_string_set(mag->file_name, mag->text_store);

            if(mag_device_save(mag->mag_dev, furi_string_get_cstr(mag->file_name))) {
                scene_manager_next_scene(scene_manager, MagSceneSaveSuccess);
            } else {
                //scene_manager_search_and_switch_to_previous_scene(
                //    scene_manager, MagSceneReadKeyMenu);
                // TODO: Replace with appropriate scene! No read scene prior if adding manually...
            }
        }
    }

    return consumed;
}

void mag_scene_input_name_on_exit(void* context) {
    Mag* mag = context;
    TextInput* text_input = mag->text_input;

    void* validator_context = text_input_get_validator_callback_context(text_input);
    text_input_set_validator(text_input, NULL, NULL);
    validator_is_file_free((ValidatorIsFile*)validator_context);

    text_input_reset(text_input);
}
