#include "../seader_i.h"
#include <lib/toolbox/random_name.h>
#include <gui/modules/validators.h>
#include <toolbox/path.h>

void seader_scene_save_name_text_input_callback(void* context) {
    Seader* seader = context;

    view_dispatcher_send_custom_event(seader->view_dispatcher, SeaderCustomEventTextInputDone);
}

void seader_scene_save_name_on_enter(void* context) {
    Seader* seader = context;

    // Setup view
    TextInput* text_input = seader->text_input;
    bool cred_name_empty = false;
    if(!strcmp(seader->credential->name, "")) {
        set_random_name(seader->text_store, sizeof(seader->text_store));
        cred_name_empty = true;
    } else {
        seader_text_store_set(seader, seader->credential->name);
    }
    text_input_set_header_text(text_input, "Name the credential");
    text_input_set_result_callback(
        text_input,
        seader_scene_save_name_text_input_callback,
        seader,
        seader->text_store,
        SEADER_CRED_NAME_MAX_LEN,
        cred_name_empty);

    FuriString* folder_path = furi_string_alloc();
    if(furi_string_end_with(seader->credential->load_path, SEADER_APP_EXTENSION)) {
        path_extract_dirname(furi_string_get_cstr(seader->credential->load_path), folder_path);
    } else {
        furi_string_set(folder_path, STORAGE_APP_DATA_PATH_PREFIX);
    }

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path), SEADER_APP_EXTENSION, seader->credential->name);
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewTextInput);

    furi_string_free(folder_path);
}

bool seader_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SeaderCustomEventTextInputDone) {
            if(strcmp(seader->credential->name, "") != 0) {
                seader_credential_delete(seader->credential, true);
            }
            strlcpy(seader->credential->name, seader->text_store, strlen(seader->text_store) + 1);
            if(seader_credential_save(seader->credential, seader->text_store)) {
                scene_manager_next_scene(seader->scene_manager, SeaderSceneSaveSuccess);
                consumed = true;
            } else {
                consumed = scene_manager_search_and_switch_to_previous_scene(
                    seader->scene_manager, SeaderSceneSamPresent);
            }
        }
    }
    return consumed;
}

void seader_scene_save_name_on_exit(void* context) {
    Seader* seader = context;

    // Clear view
    void* validator_context = text_input_get_validator_callback_context(seader->text_input);
    text_input_set_validator(seader->text_input, NULL, NULL);
    validator_is_file_free(validator_context);

    text_input_reset(seader->text_input);
}
