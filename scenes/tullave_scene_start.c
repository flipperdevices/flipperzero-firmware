#include "../tullave_i.h"

enum SubmenuIndex { SubmenuIndexRead };

void tullave_scene_start_submenu_callback(void* context, uint32_t index) {
    TuLlave* t_llave = context;
    view_dispatcher_send_custom_event(t_llave->view_dispatcher, index);
}

void tullave_scene_start_on_enter(void* context) {
    TuLlave* t_llave = context;

    Submenu* submenu = t_llave->submenu;
    submenu_add_item(
        submenu, "Read Info", SubmenuIndexRead, tullave_scene_start_submenu_callback, t_llave);

    //notification_message_block(t_llave->notifications, &sequence_reset_blue);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(t_llave->scene_manager, TuLlaveSceneStart));
    view_dispatcher_switch_to_view(t_llave->view_dispatcher, TuLlaveViewMenu);
}

bool tullave_scene_start_on_event(void* context, SceneManagerEvent event) {
    TuLlave* t_llave = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_set_scene_state(
                t_llave->scene_manager, TuLlaveSceneStart, SubmenuIndexRead);
            scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneRead);
            consumed = true;
        }
        // TODO: Here add other options for submenu
    }

    return consumed;
}

void tullave_scene_start_on_exit(void* context) {
    TuLlave* t_llave = context;
    submenu_reset(t_llave->submenu);
}
