#include "../infrared_i.h"

enum SubmenuIndex {
    SubmenuIndexUniversalRemotes,
    SubmenuIndexLearnNewRemote,
    SubmenuIndexSavedRemotes,
};

static void infrared_scene_start_submenu_callback(void* context, uint32_t index) {
    Infrared* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_start_on_enter(void* context) {
    Infrared* infrared = context;
    Submenu* submenu = infrared->submenu;

    submenu_add_item(
        submenu,
        "Universal Remotes",
        SubmenuIndexUniversalRemotes,
        infrared_scene_start_submenu_callback,
        infrared);
    submenu_add_item(
        submenu,
        "Learn New Remote",
        SubmenuIndexLearnNewRemote,
        infrared_scene_start_submenu_callback,
        infrared);
    submenu_add_item(
        submenu,
        "Saved Remotes",
        SubmenuIndexSavedRemotes,
        infrared_scene_start_submenu_callback,
        infrared);

    submenu_set_selected_item(submenu, SubmenuIndexUniversalRemotes);
    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewSubmenu);
}

bool infrared_scene_start_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexUniversalRemotes) {
            scene_manager_next_scene(scene_manager, InfraredSceneUniversal);
            consumed = true;
        } else if(event.event == SubmenuIndexLearnNewRemote) {
            infrared->app_state.is_learning_new_remote = true;
            scene_manager_next_scene(scene_manager, InfraredSceneLearn);
            consumed = true;
        } else if(event.event == SubmenuIndexSavedRemotes) {
            scene_manager_next_scene(scene_manager, InfraredSceneRemoteList);
            consumed = true;
        }
    }

    return consumed;
}

void infrared_scene_start_on_exit(void* context) {
    Infrared* infrared = context;
    submenu_reset(infrared->submenu);
}
