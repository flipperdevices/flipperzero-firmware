#include "../nfc_magic_app_i.h"
#include "protocols/gen4/gen4_poller_i.h"

enum SubmenuIndex {
    SubmenuIndexPreWriteMode,
    SubmenuIndexRestoreMode,
    SubmenuIndexDisable,
    SubmenuIndexDisableHighSpeed,
    SubmenuIndexSplitMode,
};

void nfc_magic_scene_gen4_select_shd_mode_submenu_callback(void* context, uint32_t index) {
    NfcMagicApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_magic_scene_gen4_select_shd_mode_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Pre-Write",
        SubmenuIndexPreWriteMode,
        nfc_magic_scene_gen4_select_shd_mode_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Restore",
        SubmenuIndexRestoreMode,
        nfc_magic_scene_gen4_select_shd_mode_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Disable",
        SubmenuIndexDisable,
        nfc_magic_scene_gen4_select_shd_mode_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Disable (High-Speed)",
        SubmenuIndexDisableHighSpeed,
        nfc_magic_scene_gen4_select_shd_mode_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Split",
        SubmenuIndexSplitMode,
        nfc_magic_scene_gen4_select_shd_mode_submenu_callback,
        instance);

    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneGen4SelectShdMode));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewMenu);
}

bool nfc_magic_scene_gen4_select_shd_mode_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexPreWriteMode) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetShdMode,
                Gen4PollerShadowModePreWrite);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexRestoreMode) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcMagicSceneGen4SetShdMode, Gen4PollerShadowModeRestore);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexDisable) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetShdMode,
                Gen4PollerShadowModeDisabled);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexDisableHighSpeed) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetShdMode,
                Gen4PollerShadowModeHighSpeedDisabled);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexSplitMode) {
            scene_manager_set_scene_state(
                instance->scene_manager, NfcMagicSceneGen4SetShdMode, Gen4PollerShadowModeSplit);
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SetShdMode);
            consumed = true;
        }
        scene_manager_set_scene_state(
            instance->scene_manager, NfcMagicSceneGen4SelectShdMode, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneGen4ActionsMenu);
    }

    return consumed;
}

void nfc_magic_scene_gen4_select_shd_mode_on_exit(void* context) {
    NfcMagicApp* instance = context;

    submenu_reset(instance->submenu);
}
