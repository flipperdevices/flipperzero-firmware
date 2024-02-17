#include "app_i.h"

#include <furi.h>

#include <dialogs/dialogs.h>
#include <storage/storage.h>

void scene_file_select_on_enter(void* context) {
    App* app = context;
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);

    DialogsFileBrowserOptions options;
    dialog_file_browser_set_basic_options(&options, VGM_FW_FILE_EXTENSION, NULL);

    options.hide_dot_files = true;
    options.base_path = VGM_FW_DEFAULT_PATH;

    if(dialog_file_browser_show(dialogs, app->file_path, app->file_path, &options)) {
        scene_manager_next_scene(app->scene_manager, SceneConfirm);
    } else {
        furi_string_reset(app->file_path);
        scene_manager_previous_scene(app->scene_manager);
    }

    furi_record_close(RECORD_DIALOGS);
}

bool scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
