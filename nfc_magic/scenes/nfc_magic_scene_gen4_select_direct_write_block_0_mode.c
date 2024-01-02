#include "../nfc_magic_app_i.h"
#include "furi_hal_rtc.h"
#include "protocols/gen4/gen4_poller_i.h"

enum SubmenuIndex {
    SubmenuIndexEnable,
    SubmenuIndexDisable,
    SubmenuIndexDefault,
};

void nfc_magic_scene_gen4_select_direct_write_block_0_mode_submenu_callback(
    void* context,
    uint32_t index) {
    NfcMagicApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, index);
}

void nfc_magic_scene_gen4_select_direct_write_block_0_mode_on_enter(void* context) {
    NfcMagicApp* instance = context;

    Submenu* submenu = instance->submenu;
    submenu_add_item(
        submenu,
        "Enable",
        SubmenuIndexEnable,
        nfc_magic_scene_gen4_select_direct_write_block_0_mode_submenu_callback,
        instance);
    submenu_add_item(
        submenu,
        "Disable",
        SubmenuIndexDisable,
        nfc_magic_scene_gen4_select_direct_write_block_0_mode_submenu_callback,
        instance);
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        submenu_add_item(
            submenu,
            "Default",
            SubmenuIndexDefault,
            nfc_magic_scene_gen4_select_direct_write_block_0_mode_submenu_callback,
            instance);
    }
    submenu_set_selected_item(
        submenu,
        scene_manager_get_scene_state(
            instance->scene_manager, NfcMagicSceneGen4SelectDirectWriteBlock0Mode));
    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcMagicAppViewMenu);
}

bool nfc_magic_scene_gen4_select_direct_write_block_0_mode_on_event(
    void* context,
    SceneManagerEvent event) {
    NfcMagicApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexEnable) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetDirectWriteBlock0Mode,
                Gen4PollerDirectWriteBlock0ModeEnabled);
            scene_manager_next_scene(
                instance->scene_manager, NfcMagicSceneGen4SetDirectWriteBlock0Mode);
            consumed = true;
        } else if(event.event == SubmenuIndexDisable) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetDirectWriteBlock0Mode,
                Gen4PollerDirectWriteBlock0ModeDisabled);
            scene_manager_next_scene(
                instance->scene_manager, NfcMagicSceneGen4SetDirectWriteBlock0Mode);
            consumed = true;
        } else if(event.event == SubmenuIndexDefault) {
            scene_manager_set_scene_state(
                instance->scene_manager,
                NfcMagicSceneGen4SetDirectWriteBlock0Mode,
                Gen4PollerDirectWriteBlock0ModeDefault);
            scene_manager_next_scene(
                instance->scene_manager, NfcMagicSceneGen4SetDirectWriteBlock0Mode);
            consumed = true;
        }
        scene_manager_set_scene_state(
            instance->scene_manager, NfcMagicSceneGen4SelectDirectWriteBlock0Mode, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            instance->scene_manager, NfcMagicSceneGen4ActionsMenu);
    }

    return consumed;
}

void nfc_magic_scene_gen4_select_direct_write_block_0_mode_on_exit(void* context) {
    NfcMagicApp* instance = context;

    submenu_reset(instance->submenu);
}
