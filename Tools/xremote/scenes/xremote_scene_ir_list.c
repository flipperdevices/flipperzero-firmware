#include "../xremote.h"

void xremote_scene_ir_list_on_enter(void* context) {
    XRemote* app = context;
    
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, INFRARED_APP_EXTENSION, &I_ir_10px);
    browser_options.base_path = INFRARED_APP_FOLDER;

    bool success = dialog_file_browser_show(
        app->dialogs, app->file_path, app->file_path, &browser_options);

    if(success) {
        //Load Remote into buffer
    }

    if(success) {
        //Load Remote Button View
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneCreateAdd);
        scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool xremote_scene_ir_list_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;

    return consumed;
}

void xremote_scene_ir_list_on_exit(void* context) {
    UNUSED(context);
}