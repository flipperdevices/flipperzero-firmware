#include "../uhf_app_i.h"

enum SubmenuIndex {
    SubmenuIndexDelete,
    SubmenuIndexInfo,
    SubmenuIndexWrite,
};

void uhf_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    UHFApp* uhf_app = context;

    view_dispatcher_send_custom_event(uhf_app->view_dispatcher, index);
}

void uhf_scene_saved_menu_on_enter(void* context) {
    UHFApp* uhf_app = context;
    Submenu* submenu = uhf_app->submenu;

    submenu_add_item(
        submenu, "Delete", SubmenuIndexDelete, uhf_scene_saved_menu_submenu_callback, uhf_app);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, uhf_scene_saved_menu_submenu_callback, uhf_app);
    submenu_add_item(
        submenu, "Write", SubmenuIndexWrite, uhf_scene_saved_menu_submenu_callback, uhf_app);

    submenu_set_selected_item(
        uhf_app->submenu,
        scene_manager_get_scene_state(uhf_app->scene_manager, UHFSceneSavedMenu));

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewMenu);
}

bool uhf_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(uhf_app->scene_manager, UHFSceneSavedMenu, event.event);

        if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneDelete);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(uhf_app->scene_manager, UHFSceneDeviceInfo);
            consumed = true;
        }
        // } else if(event.event == SubmenuIndexWrite) {
        //     scene_manager_next_scene(uhf_app->scene_manager, UHFSceneWriteCard);
        //     consumed = true;
        // }
    }

    return consumed;
}

void uhf_scene_saved_menu_on_exit(void* context) {
    UHFApp* uhf_app = context;

    submenu_reset(uhf_app->submenu);
}
