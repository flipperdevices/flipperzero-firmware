#include "../hex_viewer.h"

void hex_viewer_scene_open_on_enter(void* context) {
    furi_assert(context);
    HexViewer* app = context;

    FuriString* initial_path;
    initial_path = furi_string_alloc();
    furi_string_set(initial_path, HEX_VIEWER_APP_PATH_FOLDER);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, HEX_VIEWER_APP_EXTENSION, &I_hex_10px);
    browser_options.hide_ext = false;

    bool success =
        dialog_file_browser_show(app->dialogs, app->file_path, initial_path, &browser_options);
    furi_string_free(initial_path);

    if(success) {
        success = hex_viewer_open_file(app, furi_string_get_cstr(app->file_path));
        if(success) hex_viewer_read_file(app);
    }

    if(success) {
        scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, HexViewerViewIdStartscreen);
    } else {
        scene_manager_previous_scene(app->scene_manager);
    }
}

bool hex_viewer_scene_open_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = true;

    return consumed;
}

void hex_viewer_scene_open_on_exit(void* context) {
    UNUSED(context);
}
