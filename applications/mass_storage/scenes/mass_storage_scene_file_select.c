#include "../mass_storage_app_i.h"
#include "furi_hal_power.h"

static bool mass_storage_file_select(MassStorageApp* mass_storage) {
    furi_assert(mass_storage);

    // Input events and views are managed by file_select
    bool res = dialog_file_select_show(
        mass_storage->dialogs,
        MASS_STORAGE_APP_PATH_FOLDER,
        "*",
        mass_storage->file_name,
        sizeof(mass_storage->file_name),
        NULL);
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
    // MassStorageApp* mass_storage = context;
    return false;
}

void mass_storage_scene_file_select_on_exit(void* context) {
    // MassStorageApp* mass_storage = context;
}
