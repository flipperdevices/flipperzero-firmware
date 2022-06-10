#include "../infrared_i.h"

typedef enum {
    SubmenuIndexAddButton,
    SubmenuIndexRenameButton,
    SubmenuIndexDeleteButton,
    SubmenuIndexRenameRemote,
    SubmenuIndexDeleteRemote,
} SubmenuIndex;

static void infrared_scene_edit_submenu_callback(void* context, uint32_t index) {
    Infrared* infrared = context;
    view_dispatcher_send_custom_event(infrared->view_dispatcher, index);
}

void infrared_scene_edit_on_enter(void* context) {
    Infrared* infrared = context;
    Submenu* submenu = infrared->submenu;

    submenu_add_item(
        submenu,
        "Add Button",
        SubmenuIndexAddButton,
        infrared_scene_edit_submenu_callback,
        context);
    submenu_add_item(
        submenu,
        "Rename Button",
        SubmenuIndexRenameButton,
        infrared_scene_edit_submenu_callback,
        context);
    submenu_add_item(
        submenu,
        "Delete Button",
        SubmenuIndexDeleteButton,
        infrared_scene_edit_submenu_callback,
        context);
    submenu_add_item(
        submenu,
        "Rename Remote",
        SubmenuIndexRenameRemote,
        infrared_scene_edit_submenu_callback,
        context);
    submenu_add_item(
        submenu,
        "Delete Remote",
        SubmenuIndexDeleteRemote,
        infrared_scene_edit_submenu_callback,
        context);
    submenu_set_selected_item(submenu, 0);

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewSubmenu);
}

bool infrared_scene_edit_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    SceneManager* scene_manager = infrared->scene_manager;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexAddButton) {
            infrared->app_state.is_learning_new_remote = false;
            scene_manager_next_scene(scene_manager, InfraredSceneLearn);
            consumed = true;
        } else if(event.event == SubmenuIndexRenameButton) {
            infrared->app_state.edit_target = InfraredEditTargetButton;
            infrared->app_state.edit_mode = InfraredEditModeRename;
            scene_manager_next_scene(scene_manager, InfraredSceneEditButtonSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexDeleteButton) {
            infrared->app_state.edit_target = InfraredEditTargetButton;
            infrared->app_state.edit_mode = InfraredEditModeDelete;
            scene_manager_next_scene(scene_manager, InfraredSceneEditButtonSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexRenameRemote) {
            infrared->app_state.edit_target = InfraredEditTargetRemote;
            infrared->app_state.edit_mode = InfraredEditModeRename;
            scene_manager_next_scene(scene_manager, InfraredSceneEditRename);
            consumed = true;
        } else if(event.event == SubmenuIndexDeleteRemote) {
            infrared->app_state.edit_target = InfraredEditTargetRemote;
            infrared->app_state.edit_mode = InfraredEditModeDelete;
            scene_manager_next_scene(scene_manager, InfraredSceneEditDelete);
            consumed = true;
        }
    }

    return consumed;
}

void infrared_scene_edit_on_exit(void* context) {
    Infrared* infrared = context;
    submenu_reset(infrared->submenu);
}
