#include "../mag_i.h"

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexEdit,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
};

void mag_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    Mag* mag = context;

    view_dispatcher_send_custom_event(mag->view_dispatcher, index);
}

void mag_scene_saved_menu_on_enter(void* context) {
    Mag* mag = context;
    Submenu* submenu = mag->submenu;

    submenu_add_item(
        submenu, "Emulate (WIP)", SubmenuIndexEmulate, mag_scene_saved_menu_submenu_callback, mag);
    submenu_add_item(
        submenu, "Edit (WIP)", SubmenuIndexEdit, mag_scene_saved_menu_submenu_callback, mag);
    submenu_add_item(
        submenu, "Delete", SubmenuIndexDelete, mag_scene_saved_menu_submenu_callback, mag);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, mag_scene_saved_menu_submenu_callback, mag);

    submenu_set_selected_item(
        mag->submenu, scene_manager_get_scene_state(mag->scene_manager, MagSceneSavedMenu));

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewSubmenu);
}

bool mag_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(mag->scene_manager, MagSceneSavedMenu, event.event);

        // TODO: replace with actual next scenes once built
        if(event.event == SubmenuIndexEmulate) {
            scene_manager_next_scene(mag->scene_manager, MagSceneEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexEdit) {
            scene_manager_next_scene(mag->scene_manager, MagSceneUnderConstruction);
            consumed = true;
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(mag->scene_manager, MagSceneDeleteConfirm);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(mag->scene_manager, MagSceneSavedInfo);
            consumed = true;
        }
    }

    return consumed;
}

void mag_scene_saved_menu_on_exit(void* context) {
    Mag* mag = context;

    submenu_reset(mag->submenu);
}
