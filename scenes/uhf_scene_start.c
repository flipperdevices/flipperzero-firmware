#include "../uhf_app_i.h"

enum SubmenuIndex { SubmenuIndexRead, SubmenuIndexSaved, SubmenuIndexSettings };

void uhf_scene_start_submenu_callback(void* ctx, uint32_t index) {
    UHFApp* uhf_app = ctx;
    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, index);
}

void uhf_scene_start_on_enter(void* ctx) {
    UHFApp* uhf_app = ctx;

    Submenu* submenu = uhf_app->submenu;
    submenu_add_item(
        submenu, "Read Tag", SubmenuIndexRead, uhf_scene_start_submenu_callback, uhf_app);
    submenu_add_item(
        submenu, "Saved", SubmenuIndexSaved, uhf_scene_start_submenu_callback, uhf_app);
    submenu_add_item(
        submenu, "Settings", SubmenuIndexSettings, uhf_scene_start_submenu_callback, uhf_app);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(uhf_app->scene_manager, UHFSceneStart));
    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
}

bool uhf_scene_start_on_event(void* ctx, SceneManagerEvent event) {
    UHFApp* uhf_app = ctx;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_set_scene_state(uhf_app->scene_manager, UHFSceneStart, SubmenuIndexRead);
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneReadTag);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            // Explicitly save state so that the correct item is
            // reselected if the user cancels loading a file.
            scene_manager_set_scene_state(
                uhf_app->scene_manager, UHFSceneStart, SubmenuIndexSaved);
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneFileSelect);
            consumed = true;
        }
        // } else if(event.event == SubmenuIndexEliteDictAttack) {
        //     scene_manager_set_scene_state(
        //         uhf_app->scene_manager, UHFSceneStart, SubmenuIndexEliteDictAttack);
        //     scene_manager_next_scene(uhf_app->scene_manager, UHFSceneEliteDictAttack);
        //     consumed = true;
        // }
        // consumed = true;
    }
    return consumed;
}

void uhf_scene_start_on_exit(void* ctx) {
    UHFApp* uhf_app = ctx;
    submenu_reset(uhf_app->submenu);
}