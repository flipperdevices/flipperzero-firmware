#include "../tullave_co_i.h"

enum SubmenuIndex { SubmenuIndexCheck };

void tullave_co_scene_start_submenu_callback(void* context, uint32_t index) {
    TuLlave* t_llave = context;
    view_dispatcher_send_custom_event(t_llave->view_dispatcher, index);
}

void tullave_co_scene_start_on_enter(void* context) {
    TuLlave* t_llave = context;

    Submenu* submenu = t_llave->submenu;
    submenu_add_item(
        submenu,
        "Check TuLlave",
        SubmenuIndexCheck,
        tullave_co_scene_start_submenu_callback,
        t_llave);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(t_llave->scene_manager, TuLlaveSceneStart));
    view_dispatcher_switch_to_view(t_llave->view_dispatcher, TuLlaveViewMenu);
}

bool tullave_co_scene_start_on_event(void* context, SceneManagerEvent event) {
    TuLlave* t_llave = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexCheck) {
            scene_manager_set_scene_state(
                t_llave->scene_manager, TuLlaveSceneStart, SubmenuIndexCheck);
            scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneCheck);
            consumed = true;
        }
        // TODO: Here add other options for submenu
    }

    return consumed;
}

void tullave_co_scene_start_on_exit(void* context) {
    TuLlave* t_llave = context;
    submenu_reset(t_llave->submenu);
}
