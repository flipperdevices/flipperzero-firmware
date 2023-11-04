#include "../xremote.h"

#include <string.h>
#include <toolbox/path.h>

void xremote_scene_save_remote_on_enter(void* context) {
    XRemote* app = context;
    CrossRemote* remote = app->cross_remote;
    TextInput* text_input = app->text_input;
    size_t enter_name_length = 0;

    text_input_set_header_text(text_input, "Name the remote");
    enter_name_length = XREMOTE_MAX_REMOTE_NAME_LENGTH;

    FuriString* folder_path;
    folder_path = furi_string_alloc();

    //if(furi_string_end_with(app->))

    //A lot missing here

    ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
        furi_string_get_cstr(folder_path), XREMOTE_APP_EXTENSION, cross_remote_get_name(remote));
    text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

    furi_string_free(folder_path);

    text_input_set_result_callback(
        text_input,
        xremote_text_input_callback,
        context,
        app->text_store[0],
        enter_name_length,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdTextInput);
}

bool xremote_scene_save_remote_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    CrossRemote* remote = app->cross_remote;
    SceneManager* scene_manager = app->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        bool success = false;

        success = cross_remote_save_new(remote, app->text_store[0]);

        if(success) {
            scene_manager_next_scene(scene_manager, XRemoteSceneMenu);
        } else {
            scene_manager_search_and_switch_to_previous_scene(scene_manager, XRemoteSceneCreate);
        }
        consumed = true;
    }
    return consumed;
}

void xremote_scene_save_remote_on_exit(void* context) {
    XRemote* app = context;
    TextInput* text_input = app->text_input;

    void* validator_context = text_input_get_validator_callback_context(text_input);
    text_input_set_validator(text_input, NULL, NULL);

    size_t enter_name_length = XREMOTE_MAX_REMOTE_NAME_LENGTH;
    strncpy(app->text_store[0], "", enter_name_length);
    if(validator_context) {
        validator_is_file_free((ValidatorIsFile*)validator_context);
    }
}