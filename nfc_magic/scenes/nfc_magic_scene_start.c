#include "../nfc_magic_app_i.h"

enum SubmenuIndex {
    SubmenuIndexCheck,
    SubmenuIndexGen4ActionsMenu,
};

void nfc_magic_scene_start_submenu_callback(void* context, uint32_t index) {
    NfcMagicApp* instance = context;
    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_magic_scene_start_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Check Magic Tag",
        SubmenuIndexCheck,
        nfc_magic_scene_start_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Gen4 Actions",
        SubmenuIndexGen4ActionsMenu,
        nfc_magic_scene_start_submenu_callback,
        instance);

    instance->gen4_password = 0;

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneStart));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewMenu);
}

bool nfc_magic_scene_start_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexCheck) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcMagicSceneStart, SubmenuIndexCheck);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneCheck);
            consumed = true;
        } else if(event.event == SubmenuIndexGen4ActionsMenu) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4ActionsMenu);
        }
    }

    return consumed;
}

void nfc_magic_scene_start_on_exit(void* context) {
    NfcMagicApp* instance = context;

    submenu_reset(instance->submenu);
}
