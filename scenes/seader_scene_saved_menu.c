#include "../seader_i.h"

enum SubmenuIndex {
    SubmenuIndexInfo,
    SubmenuIndexDelete,
    SubmenuIndexVirtual,
};

void seader_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    Seader* seader = context;

    view_dispatcher_send_custom_event(seader->view_dispatcher, index);
}

void seader_scene_saved_menu_on_enter(void* context) {
    Seader* seader = context;
    SeaderCredential* credential = seader->credential;
    Submenu* submenu = seader->submenu;

    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, seader_scene_saved_menu_submenu_callback, seader);
    submenu_add_item(
        submenu, "Delete", SubmenuIndexDelete, seader_scene_saved_menu_submenu_callback, seader);

    if(credential->sio[0] == 0x30) {
        submenu_add_item(
            submenu,
            "Virtual",
            SubmenuIndexVirtual,
            seader_scene_saved_menu_submenu_callback,
            seader);
    }

    submenu_set_selected_item(
        seader->submenu,
        scene_manager_get_scene_state(seader->scene_manager, SeaderSceneSavedMenu));

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewMenu);
}

bool seader_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(seader->scene_manager, SeaderSceneSavedMenu, event.event);

        if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneDelete);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneCredentialInfo);
            consumed = true;
        } else if(event.event == SubmenuIndexVirtual) {
            scene_manager_next_scene(seader->scene_manager, SeaderSceneVirtualCredential);
            consumed = true;
        }
    }

    return consumed;
}

void seader_scene_saved_menu_on_exit(void* context) {
    Seader* seader = context;

    submenu_reset(seader->submenu);
}
