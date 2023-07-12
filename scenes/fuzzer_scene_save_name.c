#include "../fuzzer_i.h"

#include <toolbox/random_name.h>
#include <toolbox/path.h>

static void fuzzer_scene_save_name_text_input_callback(void* context) {
    PacsFuzzerApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, FuzzerCustomEventTextEditResult);
}

void fuzzer_scene_save_name_on_enter(void* context) {
    PacsFuzzerApp* app = context;
    TextInput* text_input = app->text_input;

    set_random_name(app->key_name, KEY_NAME_SIZE);

    text_input_set_header_text(text_input, "Name the key");
    text_input_set_result_callback(
        text_input,
        fuzzer_scene_save_name_text_input_callback,
        app,
        app->key_name,
        KEY_NAME_SIZE,
        false);

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        app->fuzzer_const->path_key_folder, app->fuzzer_const->key_extension, app->key_name);
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    view_dispatcher_switch_to_view(app->view_dispatcher, FuzzerViewIDTextInput);
}

bool fuzzer_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    PacsFuzzerApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == FuzzerCustomEventTextEditResult) {
            consumed = true;
            furi_string_printf(
                app->file_path,
                "%s/%s%s",
                app->fuzzer_const->path_key_folder,
                app->key_name,
                app->fuzzer_const->key_extension);

            if(fuzzer_worker_save_key(app->worker, furi_string_get_cstr(app->file_path))) {
                scene_manager_next_scene(app->scene_manager, FuzzerSceneSaveSuccess);
            } else {
                scene_manager_previous_scene(app->scene_manager);
            }
        }
    }

    return consumed;
}

void fuzzer_scene_save_name_on_exit(void* context) {
    PacsFuzzerApp* app = context;
    TextInput* text_input = app->text_input;

    void* validator_context = text_input_get_validator_callback_context(text_input);
    text_input_set_validator(text_input, NULL, NULL);
    validator_is_file_free((ValidatorIsFile*)validator_context);

    text_input_reset(text_input);
}
