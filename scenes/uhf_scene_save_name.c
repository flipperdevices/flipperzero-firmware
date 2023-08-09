#include "../uhf_app_i.h"
#include <lib/toolbox/random_name.h>
#include <gui/modules/validators.h>
#include <toolbox/path.h>

void uhf_scene_save_name_text_input_callback(void* context) {
    UHFApp* uhf_app = context;

    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, UHFCustomEventTextInputDone);
}

void uhf_scene_save_name_on_enter(void* context) {
    UHFApp* uhf_app = context;

    // Setup view
    TextInput* text_input = uhf_app->text_input;
    set_random_name(uhf_app->text_store, sizeof(uhf_app->text_store));
    text_input_set_header_text(text_input, "Name the tag");
    text_input_set_result_callback(
        text_input,
        uhf_scene_save_name_text_input_callback,
        uhf_app,
        uhf_app->text_store,
        UHF_DEV_NAME_MAX_LEN,
        true);

    FuriString* folder_path;
    folder_path = furi_string_alloc_set(STORAGE_APP_DATA_PATH_PREFIX);

    if(furi_string_end_with(uhf_app->uhf_device->load_path, UHF_APP_EXTENSION)) {
        path_extract_dirname(furi_string_get_cstr(uhf_app->uhf_device->load_path), folder_path);
    }

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path), UHF_APP_EXTENSION, uhf_app->uhf_device->dev_name);
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewTextInput);

    furi_string_free(folder_path);
}

bool uhf_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == UHFCustomEventTextInputDone) {
            strlcpy(
                uhf_app->uhf_device->dev_name,
                uhf_app->text_store,
                strlen(uhf_app->text_store) + 1);
            if(uhf_device_save(uhf_app->uhf_device, uhf_app->text_store)) {
                scene_manager_next_scene(uhf_app->scene_manager, UHFSceneSaveSuccess);
                consumed = true;
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    uhf_app->scene_manager, UHFSceneStart);
            }
        }
    }
    return consumed;
}

void uhf_scene_save_name_on_exit(void* context) {
    UHFApp* uhf_app = context;

    // Clear view
    void* validator_context = text_input_get_validator_callback_context(uhf_app->text_input);
    text_input_set_validator(uhf_app->text_input, NULL, NULL);
    validator_is_file_free(validator_context);

    text_input_reset(uhf_app->text_input);
}
