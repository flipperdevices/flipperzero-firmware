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

    if(event.type != SceneManagerEventTypeCustom) {
        return false;
    } else if(event.event == SubmenuIndexAddButton) {
        // Learn new remote = false
        scene_manager_next_scene(scene_manager, InfraredSceneLearn);
    } else if(event.event == SubmenuIndexRenameButton) {
        // Action = rename
        // Element = button
        scene_manager_next_scene(scene_manager, InfraredSceneEditButtonSelect);
    } else if(event.event == SubmenuIndexDeleteButton) {
        // Action = delete
        // Element = button
        scene_manager_next_scene(scene_manager, InfraredSceneEditButtonSelect);
    } else if(event.event == SubmenuIndexRenameRemote) {
        // Action = rename
        // Element = remote
        scene_manager_next_scene(scene_manager, InfraredSceneEditRename);
    } else if(event.event == SubmenuIndexDeleteRemote) {
        // Action = delete
        // Element = remote
        scene_manager_next_scene(scene_manager, InfraredSceneEditDelete);
    } else {
        return false;
    }

    return true;
}

void infrared_scene_edit_on_exit(void* context) {
    Infrared* infrared = context;
    submenu_reset(infrared->submenu);
}
