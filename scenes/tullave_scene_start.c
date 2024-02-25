#include "../tullave_i.h"

enum SubmenuIndex { SubmenuIndexRead };

void tullave_scene_start_submenu_callback(void* context, uint32_t index) {
    TuLlaveApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void tullave_scene_start_on_enter(void* context) {
    TuLlaveApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu, "Read Info", SubmenuIndexRead, tullave_scene_start_submenu_callback, instance);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, TuLlaveSceneStart));
    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewMenu);
}

bool tullave_scene_start_on_event(void* context, SceneManagerEvent event) {
    TuLlaveApp* t_llave = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_set_scene_state(
                t_llave->scene_manager, TuLlaveSceneStart, SubmenuIndexRead);
            scene_manager_next_scene(t_llave->scene_manager, TuLlaveSceneDetect);
            consumed = true;
        }
        // TODO: Here add other options for submenu
    }

    return consumed;
}

void tullave_scene_start_on_exit(void* context) {
    TuLlaveApp* t_llave = context;
    submenu_reset(t_llave->submenu);
}
