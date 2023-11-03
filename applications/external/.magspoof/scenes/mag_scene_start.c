#include "../mag_i.h"

typedef enum {
    SubmenuIndexSaved,
    SubmenuIndexRead,
    //SubmenuIndexAddManually,
    SubmenuIndexAbout,
} SubmenuIndex;

static void mag_scene_start_submenu_callback(void* context, uint32_t index) {
    Mag* mag = context;

    view_dispatcher_send_custom_event(mag->view_dispatcher, index);
}

void mag_scene_start_on_enter(void* context) {
    Mag* mag = context;
    Submenu* submenu = mag->submenu;

    submenu_add_item(submenu, "Saved", SubmenuIndexSaved, mag_scene_start_submenu_callback, mag);
    submenu_add_item(submenu, "Read", SubmenuIndexRead, mag_scene_start_submenu_callback, mag);
    //submenu_add_item(
    //    submenu, "Add Manually", SubmenuIndexAddManually, mag_scene_start_submenu_callback, mag);
    submenu_add_item(submenu, "About", SubmenuIndexAbout, mag_scene_start_submenu_callback, mag);

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
        switch(event.event) {
        case SubmenuIndexSaved:
            furi_string_set(mag->file_path, MAG_APP_FOLDER);
            scene_manager_next_scene(mag->scene_manager, MagSceneFileSelect);
            consumed = true;
            break;

        case SubmenuIndexRead:
            scene_manager_next_scene(mag->scene_manager, MagSceneRead);
            consumed = true;
            break;
        //case SubmenuIndexAddManually:
        //    scene_manager_next_scene(mag->scene_manager, MagSceneInputValue);
        //    consumed = true;
        //    break;
        case SubmenuIndexAbout:
            scene_manager_next_scene(mag->scene_manager, MagSceneAbout);
            consumed = true;
            break;
        }

        scene_manager_set_scene_state(mag->scene_manager, MagSceneStart, event.event);
    }

    return consumed;
}

void mag_scene_start_on_exit(void* context) {
    Mag* mag = context;

    submenu_reset(mag->submenu);
}