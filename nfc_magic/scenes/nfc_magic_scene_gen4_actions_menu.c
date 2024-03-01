#include "../nfc_magic_app_i.h"

enum SubmenuIndex {
    SubmenuIndexAuthenticate,
    SubmenuIndexSetStandartConfig,
};

void nfc_magic_scene_gen4_actions_menu_submenu_callback(void* context, uint32_t index) {
    NfcMagicApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_magic_scene_gen4_actions_menu_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Auth With Password",
        SubmenuIndexAuthenticate,
        nfc_magic_scene_gen4_actions_menu_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Set Standart Config",
        SubmenuIndexSetStandartConfig,
        nfc_magic_scene_gen4_actions_menu_submenu_callback,
        instance);

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneGen4ActionsMenu));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewMenu);
}

bool nfc_magic_scene_gen4_actions_menu_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexAuthenticate) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneKeyInput);
            consumed = true;
        } else if(event.event == SubmenuIndexSetStandartConfig) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetCfg);
            consumed = true;
        }

        scene_manager_set_scene_state(
            instance->scene_manager, NfcMagicSceneGen4ActionsMenu, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneStart);
    }

    return consumed;
}

void nfc_magic_scene_gen4_actions_menu_on_exit(void* context) {
    NfcMagicApp* instance = context;

    submenu_reset(instance->submenu);
}
