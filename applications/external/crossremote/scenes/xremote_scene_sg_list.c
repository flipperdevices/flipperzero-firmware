#include "../xremote.h"

void xremote_scene_sg_list_on_enter(void* context) {
    XRemote* app = context;

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, SUBGHZ_APP_EXTENSION, &I_sg_10px);
    browser_options.base_path = SUBGHZ_APP_FOLDER;

    FuriString* path;
    path = furi_string_alloc();
    furi_string_set(path, SUBGHZ_APP_FOLDER);
    bool success = dialog_file_browser_show(app->dialogs, app->file_path, path, &browser_options);
    furi_string_free(path);

    if(success) {
        //Load Remote into buffer
        view_set_orientation(view_stack_get_view(app->view_stack), ViewOrientationVertical);
        view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdStack);

        xremote_show_loading_popup(app, true);
        xremote_sg_remote_load(app->sg_remote_buffer, app->file_path);
        //xremote_ir_remote_load(app->ir_remote_buffer, app->file_path);
        xremote_show_loading_popup(app, false);
        cross_remote_add_subghz(app->cross_remote, app->sg_remote_buffer);
    }

    if(success) {
        //Load Remote Button View
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneCreateAdd);
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
        scene_manager_next_scene(app->scene_manager, XRemoteSceneCreate);
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneIrRemote);
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool xremote_scene_sg_list_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = true;

    return consumed;
}

void xremote_scene_sg_list_on_exit(void* context) {
    UNUSED(context);
}