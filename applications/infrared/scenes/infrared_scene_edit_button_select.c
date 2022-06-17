#include "../infrared_i.h"

static void infrared_scene_edit_button_select_submenu_callback(void* context, uint32_t index) {
    Infrared* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_edit_button_select_on_enter(void* context) {
    Infrared* infrared = context;
    Submenu* submenu = infrared->submenu;
    InfraredRemote* remote = infrared->remote;
    SceneManager* scene_manager = infrared->scene_manager;

    const char* header = infrared->app_state.edit_mode == InfraredEditModeRename ?
                             "Rename Button:" :
                             "Delete Button:";
    submenu_set_header(submenu, header);

    const size_t button_count = infrared_remote_get_button_count(remote);
    for(size_t i = 0; i < button_count; ++i) {
        InfraredRemoteButton* button = infrared_remote_get_button(remote, i);
        submenu_add_item(
            submenu,
            infrared_remote_button_get_name(button),
            i,
            infrared_scene_edit_button_select_submenu_callback,
            context);
    }

    const int32_t submenu_index =
        (signed)scene_manager_get_scene_state(scene_manager, InfraredSceneEditButtonSelect);

    if(button_count && submenu_index != InfraredButtonIndexNone) {
        submenu_set_selected_item(submenu, submenu_index);
        infrared->app_state.current_button_index = InfraredButtonIndexNone;
        scene_manager_set_scene_state(
            scene_manager, InfraredSceneEditButtonSelect, (unsigned)InfraredButtonIndexNone);
    }

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewSubmenu);
}

bool infrared_scene_edit_button_select_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        const uint32_t button_index = event.event;
        infrared->app_state.current_button_index = button_index;
        scene_manager_set_scene_state(scene_manager, InfraredSceneEditButtonSelect, button_index);

        const InfraredEditMode edit_mode = infrared->app_state.edit_mode;
        if(edit_mode == InfraredEditModeRename) {
            scene_manager_next_scene(scene_manager, InfraredSceneEditRename);
        } else if(edit_mode == InfraredEditModeDelete) {
            scene_manager_next_scene(scene_manager, InfraredSceneEditDelete);
        } else {
            furi_assert(0);
        }
        consumed = true;
    }

    return consumed;
}

void infrared_scene_edit_button_select_on_exit(void* context) {
    Infrared* infrared = context;
    submenu_reset(infrared->submenu);
}
