#include "../mass_storage_app_i.h"
#include "furi_hal_power.h"

static bool mass_storage_file_select(MassStorageApp* mass_storage) {
    furi_assert(mass_storage);

    DialogsFileBrowserOptions browser_options;
    dialog_file_browser_set_basic_options(
        &browser_options, MASS_STORAGE_APP_EXTENSION, &I_mass_storage_10px);
    browser_options.base_path = MASS_STORAGE_APP_PATH_FOLDER;
    browser_options.hide_ext = false;

    // Input events and views are managed by file_select
    bool res = dialog_file_browser_show(
        mass_storage->dialogs, mass_storage->file_path, mass_storage->file_path, &browser_options);
    return res;
}

void mass_storage_scene_file_select_on_enter(void* context) {
    MassStorageApp* mass_storage = context;

    if(mass_storage_file_select(mass_storage)) {
        if(!furi_hal_usb_is_locked()) {
            scene_manager_next_scene(mass_storage->scene_manager, MassStorageSceneWork);
        } else {
            scene_manager_next_scene(mass_storage->scene_manager, MassStorageSceneUsbLocked);
        }
    } else {
        scene_manager_previous_scene(mass_storage->scene_manager);
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
}
