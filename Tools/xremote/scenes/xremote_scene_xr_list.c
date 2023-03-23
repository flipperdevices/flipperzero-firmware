#include "../xremote.h"

void xremote_scene_xr_list_on_enter(void* context) {
    XRemote* app = context;
    
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, XREMOTE_APP_EXTENSION, &I_xr_10px);
    browser_options.base_path = XREMOTE_APP_FOLDER;

    FuriString* path;
    path = furi_string_alloc();
    furi_string_set(path, XREMOTE_APP_FOLDER);

    bool success = dialog_file_browser_show(
        //app->dialogs, app->file_path, app->file_path, &browser_options);
        app->dialogs, app->file_path, path, &browser_options);
    furi_string_free(path);

    if(success) {
        //Load Remote into buffer
        success = cross_remote_load(app->cross_remote, app->file_path);
    }

    if(success) {
        //Load Remote Button View
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
        scene_manager_next_scene(app->scene_manager, XRemoteSceneTransmit);
    } else {
        //scene_manager_next_scene(app->scene_manager, XRemoteSceneWip);
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool xremote_scene_xr_list_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = true;

    return consumed;
}

void xremote_scene_xr_list_on_exit(void* context) {
    UNUSED(context);
}