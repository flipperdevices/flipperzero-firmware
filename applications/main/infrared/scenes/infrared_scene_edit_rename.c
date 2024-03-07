#include "../infrared_app_i.h"

#include <string.h>

#include <toolbox/path.h>
#include <toolbox/concurrent_runner.h>

#define REMOTE_RENAME_STACK_SIZE (2048UL)

static void infrared_scene_edit_rename_button_callback(void* context) {
    InfraredApp* infrared = context;
    infrared->app_state.is_task_success = infrared_remote_rename_signal(
        infrared->remote, infrared->app_state.current_button_index, infrared->text_store[0]);
}

static void infrared_scene_edit_rename_remote_callback(void* context) {
    InfraredApp* infrared = context;
    infrared->app_state.is_task_success =
        infrared_rename_current_remote(infrared, infrared->text_store[0]);
}

static void infrared_scene_edit_rename_finished_callback(void* context) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(
        infrared->view_dispatcher, InfraredCustomEventTypeTaskFinished);
}

void infrared_scene_edit_rename_on_enter(void* context) {
    InfraredApp* infrared = context;
    InfraredRemote* remote = infrared->remote;
    TextInput* text_input = infrared->text_input;
    size_t enter_name_length = 0;

    const InfraredEditTarget edit_target = infrared->app_state.edit_target;
    if(edit_target == InfraredEditTargetButton) {
        text_input_set_header_text(text_input, "Name the button");

        const int32_t current_button_index = infrared->app_state.current_button_index;
        furi_check(current_button_index != InfraredButtonIndexNone);

        enter_name_length = INFRARED_MAX_BUTTON_NAME_LENGTH;
        strncpy(
            infrared->text_store[0],
            infrared_remote_get_signal_name(remote, current_button_index),
            enter_name_length);

    } else if(edit_target == InfraredEditTargetRemote) {
        text_input_set_header_text(text_input, "Name the remote");
        enter_name_length = INFRARED_MAX_REMOTE_NAME_LENGTH;
        strncpy(infrared->text_store[0], infrared_remote_get_name(remote), enter_name_length);

        FuriString* folder_path;
        folder_path = furi_string_alloc();

        if(furi_string_end_with(infrared->file_path, INFRARED_APP_EXTENSION)) {
            path_extract_dirname(furi_string_get_cstr(infrared->file_path), folder_path);
        }

        ValidatorIsFile* validator_is_file = validator_is_file_alloc_init(
            furi_string_get_cstr(folder_path),
            INFRARED_APP_EXTENSION,
            infrared_remote_get_name(remote));
        text_input_set_validator(text_input, validator_is_file_callback, validator_is_file);

        furi_string_free(folder_path);
    } else {
        furi_crash();
    }

    text_input_set_result_callback(
        text_input,
        infrared_text_input_callback,
        context,
        infrared->text_store[0],
        enter_name_length,
        false);

    view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationHorizontal);
    view_stack_add_view(infrared->view_stack, text_input_get_view(infrared->text_input));

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);
}

bool infrared_scene_edit_rename_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        InfraredAppState* app_state = &infrared->app_state;
        const InfraredEditTarget edit_target = app_state->edit_target;

        if(event.event == InfraredCustomEventTypeTextEditDone) {
            view_stack_add_view(infrared->view_stack, loading_get_view(infrared->loading));

            if(edit_target == InfraredEditTargetButton) {
                furi_assert(app_state->current_button_index != InfraredButtonIndexNone);
                // Rename button in a separate thread
                concurrent_runner_start(
                    REMOTE_RENAME_STACK_SIZE,
                    infrared_scene_edit_rename_button_callback,
                    infrared_scene_edit_rename_finished_callback,
                    infrared);
            } else if(edit_target == InfraredEditTargetRemote) {
                // Rename remote in a separate thread (for consistency)
                concurrent_runner_start(
                    REMOTE_RENAME_STACK_SIZE,
                    infrared_scene_edit_rename_remote_callback,
                    infrared_scene_edit_rename_finished_callback,
                    infrared);
            } else {
                furi_crash();
            }

        } else if(event.event == InfraredCustomEventTypeTaskFinished) {
            view_stack_remove_view(infrared->view_stack, loading_get_view(infrared->loading));

            if(app_state->is_task_success) {
                scene_manager_next_scene(scene_manager, InfraredSceneEditRenameDone);
            } else {
                infrared_show_error_message(
                    infrared,
                    "Failed to\nrename %s",
                    edit_target == InfraredEditTargetButton ? "button" : "file");
                scene_manager_search_and_switch_to_previous_scene(
                    scene_manager, InfraredSceneRemoteList);
            }

            app_state->current_button_index = InfraredButtonIndexNone;
        }
        consumed = true;
    }

    return consumed;
}

void infrared_scene_edit_rename_on_exit(void* context) {
    InfraredApp* infrared = context;
    TextInput* text_input = infrared->text_input;

    view_stack_remove_view(infrared->view_stack, text_input_get_view(text_input));

    void* validator_context = text_input_get_validator_callback_context(text_input);
    text_input_set_validator(text_input, NULL, NULL);

    if(validator_context) {
        validator_is_file_free((ValidatorIsFile*)validator_context);
    }
}
