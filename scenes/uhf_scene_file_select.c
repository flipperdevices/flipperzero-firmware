#include "../uhf_app_i.h"

void uhf_scene_file_select_on_enter(void* context) {
    UHFApp* uhf_app = context;
    // Process file_select return
    uhf_device_set_loading_callback(uhf_app->uhf_device, uhf_show_loading_popup, uhf_app);
    if(uhf_file_select(uhf_app->uhf_device)) {
        scene_manager_next_scene(uhf_app->scene_manager, UHFSceneSavedMenu);
    } else {
        scene_manager_search_and_switch_to_previous_scene(uhf_app->scene_manager, UHFSceneStart);
    }
    uhf_device_set_loading_callback(uhf_app->uhf_device, NULL, uhf_app);
}

bool uhf_scene_file_select_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void uhf_scene_file_select_on_exit(void* context) {
    UNUSED(context);
}
