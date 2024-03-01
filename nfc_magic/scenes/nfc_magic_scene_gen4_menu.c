#include "../nfc_magic_app_i.h"
#include "furi_hal_rtc.h"

enum SubmenuIndex {
    SubmenuIndexWrite,
    SubmenuIndexChangePassword,
    SubmenuIndexSetShadowMode,
    SubmenuIndexSetDirectWriteBlock0Mode,
    SubmenuIndexGetRevision,
    SubmenuIndexGetConfig,
    SubmenuIndexWipe,
};

void nfc_magic_scene_gen4_menu_submenu_callback(void* context, uint32_t index) {
    NfcMagicApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_magic_scene_gen4_menu_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu, "Write", SubmenuIndexWrite, nfc_magic_scene_gen4_menu_submenu_callback, instance);
    submenu_add_item(
        submenu,
        "Change password",
        SubmenuIndexChangePassword,
        nfc_magic_scene_gen4_menu_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Set Shadow Mode",
        SubmenuIndexSetShadowMode,
        nfc_magic_scene_gen4_menu_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Set Gen2 Mode",
        SubmenuIndexSetDirectWriteBlock0Mode,
        nfc_magic_scene_gen4_menu_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Get Revision",
        SubmenuIndexGetRevision,
        nfc_magic_scene_gen4_menu_submenu_callback,
        instance);
    submenu_add_item(
        submenu, "Wipe", SubmenuIndexWipe, nfc_magic_scene_gen4_menu_submenu_callback, instance);
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        submenu_add_item(
            submenu,
            "Get Config",
            SubmenuIndexGetConfig,
            nfc_magic_scene_gen4_menu_submenu_callback,
            instance);
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(instance->scene_manager, NfcMagicSceneGen4Menu));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewMenu);
}

bool nfc_magic_scene_gen4_menu_on_event(void* context, SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneFileSelect);
            consumed = true;
        } else if(event.event == SubmenuIndexChangePassword) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneKeyInput);
            consumed = true;
        } else if(event.event == SubmenuIndexWipe) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneWipe);
            consumed = true;
        } else if(event.event == SubmenuIndexGetConfig) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4GetCfg);
            consumed = true;
        } else if(event.event == SubmenuIndexGetRevision) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4Revision);
            consumed = true;
        } else if(event.event == SubmenuIndexSetShadowMode) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SelectShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexSetDirectWriteBlock0Mode) {
            scene_manager_next_scene(
                instance->scene_manager, NfcMagicSceneGen4SelectDirectWriteBlock0Mode);
            consumed = true;
        }

        scene_manager_set_scene_state(instance->scene_manager, NfcMagicSceneGen4Menu, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        if(instance->gen4_password != 0) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcMagicSceneGen4ActionsMenu);
        } else {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                instance->scene_manager, NfcMagicSceneStart);
        }
    }

    return consumed;
}

void nfc_magic_scene_gen4_menu_on_exit(void* context) {
    NfcMagicApp* instance = context;

    submenu_reset(instance->submenu);
}
