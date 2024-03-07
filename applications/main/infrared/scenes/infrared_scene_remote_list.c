#include "../infrared_app_i.h"

#include <toolbox/concurrent_runner.h>

#define REMOTE_LOADER_STACK_SIZE (2048UL)

static void infrared_scene_remote_list_load_callback(void* context) {
    InfraredApp* infrared = context;
    infrared->app_state.is_task_success =
        infrared_remote_load(infrared->remote, furi_string_get_cstr(infrared->file_path));
}

static void infrared_scene_remote_list_load_finished_callback(void* context) {
    InfraredApp* infrared = context;
    view_dispatcher_send_custom_event(
        infrared->view_dispatcher, InfraredCustomEventTypeTaskFinished);
}

static void infrared_scene_remote_list_select_and_load(InfraredApp* infrared) {
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, INFRARED_APP_EXTENSION, &I_ir_10px);
    browser_options.base_path = INFRARED_APP_FOLDER;

    if(dialog_file_browser_show(
           infrared->dialogs, infrared->file_path, infrared->file_path, &browser_options)) {
        view_set_orientation(view_stack_get_view(infrared->view_stack), ViewOrientationVertical);
        view_stack_add_view(infrared->view_stack, loading_get_view(infrared->loading));
        view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewStack);
        // Load remote in background
        concurrent_runner_start(
            REMOTE_LOADER_STACK_SIZE,
            infrared_scene_remote_list_load_callback,
            infrared_scene_remote_list_load_finished_callback,
            infrared);
    } else {
        scene_manager_previous_scene(infrared->scene_manager);
    }
}

void infrared_scene_remote_list_on_enter(void* context) {
    InfraredApp* infrared = context;
    infrared_scene_remote_list_select_and_load(infrared);
}

bool infrared_scene_remote_list_on_event(void* context, SceneManagerEvent event) {
    InfraredApp* infrared = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == InfraredCustomEventTypeTaskFinished) {
            view_stack_remove_view(infrared->view_stack, loading_get_view(infrared->loading));

            if(infrared->app_state.is_task_success) {
                scene_manager_next_scene(infrared->scene_manager, InfraredSceneRemote);
            } else {
                infrared_show_error_message(
                    infrared, "Failed to load\n\"%s\"", furi_string_get_cstr(infrared->file_path));
                infrared_scene_remote_list_select_and_load(infrared);
            }
        }
        consumed = true;
    }

    return consumed;
}

void infrared_scene_remote_list_on_exit(void* context) {
    UNUSED(context);
}
