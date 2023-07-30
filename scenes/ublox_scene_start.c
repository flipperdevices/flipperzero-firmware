#include "../ublox_i.h"

enum SubmenuIndex {
    SubmenuIndexDataDisplay,
    SubmenuIndexWiring,
    SubmenuIndexAbout,
};

void ublox_scene_start_submenu_callback(void* context, uint32_t index) {
    Ublox* ublox = context;

    view_dispatcher_send_custom_event(ublox->view_dispatcher, index);
}

void ublox_scene_start_on_enter(void* context) {
    Ublox* ublox = context;
    Submenu* submenu = ublox->submenu;

    submenu_add_item(
        submenu,
        "Data Display",
        SubmenuIndexDataDisplay,
        ublox_scene_start_submenu_callback,
        ublox);
    submenu_add_item(
        submenu, "Wiring", SubmenuIndexWiring, ublox_scene_start_submenu_callback, ublox);
    submenu_add_item(
        submenu, "About", SubmenuIndexAbout, ublox_scene_start_submenu_callback, ublox);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(ublox->scene_manager, UbloxSceneStart));

    view_dispatcher_switch_to_view(ublox->view_dispatcher, UbloxViewMenu);
}

bool ublox_scene_start_on_event(void* context, SceneManagerEvent event) {
    Ublox* ublox = context;
    UNUSED(ublox);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexDataDisplay) {
            scene_manager_set_scene_state(
                ublox->scene_manager, UbloxSceneDataDisplay, SubmenuIndexDataDisplay);
            scene_manager_next_scene(ublox->scene_manager, UbloxSceneDataDisplay);
            consumed = true;
        } else if(event.event == SubmenuIndexWiring) {
            scene_manager_set_scene_state(
                ublox->scene_manager, UbloxSceneWiring, SubmenuIndexWiring);
            scene_manager_next_scene(ublox->scene_manager, UbloxSceneWiring);
            consumed = true;
        } else if(event.event == SubmenuIndexAbout) {
            scene_manager_set_scene_state(
                ublox->scene_manager, UbloxSceneAbout, SubmenuIndexAbout);
            scene_manager_next_scene(ublox->scene_manager, UbloxSceneAbout);
            consumed = true;
        }
    }

    return consumed;
}

void ublox_scene_start_on_exit(void* context) {
    Ublox* ublox = context;

    submenu_reset(ublox->submenu);
}
