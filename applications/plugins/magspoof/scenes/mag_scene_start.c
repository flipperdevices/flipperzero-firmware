#include "../mag_i.h"

typedef enum {
    SubmenuIndexSaved,
    SubmenuIndexAddManually,
} SubmenuIndex;

static void mag_scene_start_submenu_callback(void* context, uint32_t index) {
    Mag* mag = context;

    view_dispatcher_send_custom_event(mag->view_dispatcher, index);
}

void mag_scene_start_on_enter(void* context) {
    Mag* mag = context;
    Submenu* submenu = mag->submenu;

    submenu_add_item(submenu, "Saved", SubmenuIndexSaved, mag_scene_start_submenu_callback, mag);
    submenu_add_item(
        submenu, "Add Manually", SubmenuIndexAddManually, mag_scene_start_submenu_callback, mag);
    // TODO: "About" scene?

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(mag->scene_manager, MagSceneStart));

    // clear key
    furi_string_reset(mag->file_name);

    view_dispatcher_switch_to_view(mag->view_dispatcher, MagViewSubmenu);
}

bool mag_scene_start_on_event(void* context, SceneManagerEvent event) {
    Mag* mag = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexSaved) {
            furi_string_set(mag->file_path, MAG_APP_FOLDER);
            scene_manager_next_scene(mag->scene_manager, MagSceneFileSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexAddManually) {
            scene_manager_next_scene(mag->scene_manager, MagSceneInputValue);
            consumed = true;
        }
        scene_manager_set_scene_state(mag->scene_manager, MagSceneStart, event.event);
    }

    return consumed;
}

void mag_scene_start_on_exit(void* context) {
    Mag* mag = context;

    submenu_reset(mag->submenu);
}