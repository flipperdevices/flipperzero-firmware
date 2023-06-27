#include "../mass_storage_app_i.h"
#include "furi_hal_power.h"

static bool mass_storage_file_select(MassStorageApp* mass_storage) {
    furi_assert(mass_storage);

    FuriString* file_path = furi_string_alloc();
    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(&browser_options, "*", NULL);
    browser_options.base_path = MASS_STORAGE_APP_PATH_FOLDER;
    furi_string_set(file_path, MASS_STORAGE_APP_PATH_FOLDER);

    bool res =
        dialog_file_browser_show(mass_storage->dialogs, file_path, file_path, &browser_options);

    if(res) {
        strlcpy(
            mass_storage->file_name,
            furi_string_get_cstr(file_path),
            sizeof(mass_storage->file_name));
    }
    furi_string_free(file_path);
    return res;
}

void mass_storage_scene_file_select_on_enter(void* context) {
    MassStorageApp* mass_storage = context;

    if(mass_storage_file_select(mass_storage)) {
        scene_manager_next_scene(mass_storage->scene_manager, MassStorageSceneWork);
    } else {
        scene_manager_previous_scene(mass_storage->scene_manager);
        view_dispatcher_stop(mass_storage->view_dispatcher);
    }
}

bool mass_storage_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    // MassStorageApp* mass_storage = context;
    return false;
}

void mass_storage_scene_file_select_on_exit(void* context) {
    UNUSED(context);
    // MassStorageApp* mass_storage = context;
}
