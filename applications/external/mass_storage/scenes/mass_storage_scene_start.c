#include "../mass_storage_app_i.h"

static void mass_storage_scene_start_submenu_callback(void* context, uint32_t index) {
    MassStorageApp* app = context;
    scene_manager_set_scene_state(app->scene_manager, MassStorageSceneStart, index);
    scene_manager_next_scene(app->scene_manager, index);
}

void mass_storage_scene_start_on_enter(void* context) {
    MassStorageApp* app = context;
    Submenu* submenu = app->submenu;

    submenu_add_item(
        submenu,
        "Select Disc Image",
        MassStorageSceneFileSelect,
        mass_storage_scene_start_submenu_callback,
        app);

    submenu_add_item(
        submenu,
        "Create Image",
        MassStorageSceneCreateImage,
        mass_storage_scene_start_submenu_callback,
        app);
    scene_manager_set_scene_state(app->scene_manager, MassStorageSceneCreateImage, 0);

    submenu_set_header(submenu, "USB Mass Storage");
    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(app->scene_manager, MassStorageSceneStart));

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewSubmenu);
}

bool mass_storage_scene_start_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
    }

    return consumed;
}

void mass_storage_scene_start_on_exit(void* context) {
    MassStorageApp* app = context;
    submenu_reset(app->submenu);
}
