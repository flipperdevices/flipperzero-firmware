#include "../nfc_magic_app_i.h"
#include "gui/scene_manager.h"
#include "protocols/gen4/gen4.h"

enum SubmenuIndex {
    SubmenuIndexWrite,
    SubmenuIndexChangePassword,
    SubmenuIndexSetShadowMode,
    SubmenuIndexSetDirectWriteBlock0Mode,
    SubmenuIndexInfo,
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
    if(instance->gen4_data->config.data_parsed.direct_write_mode ==
       Gen4DirectWriteBlock0ModeEnabled) {
        submenu_add_item(
            submenu,
            "Disable Direct Write Mode",
            SubmenuIndexSetDirectWriteBlock0Mode,
            nfc_magic_scene_gen4_menu_submenu_callback,
            instance);
        scene_manager_set_scene_state(
            instance->scene_manager,
            NfcMagicSceneGen4SetDirectWriteBlock0Mode,
            Gen4DirectWriteBlock0ModeDisabled);
    } else {
        submenu_add_item(
            submenu,
            "Enable Direct Write Mode",
            SubmenuIndexSetDirectWriteBlock0Mode,
            nfc_magic_scene_gen4_menu_submenu_callback,
            instance);
        scene_manager_set_scene_state(
            instance->scene_manager,
            NfcMagicSceneGen4SetDirectWriteBlock0Mode,
            Gen4DirectWriteBlock0ModeEnabled);
    }

    submenu_add_item(
        submenu, "Wipe", SubmenuIndexWipe, nfc_magic_scene_gen4_menu_submenu_callback, instance);
    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, nfc_magic_scene_gen4_menu_submenu_callback, instance);

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
        } else if(event.event == SubmenuIndexInfo) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4GetInfo);
            consumed = true;
        } else if(event.event == SubmenuIndexSetShadowMode) {
            scene_manager_next_scene(instance->scene_manager, NfcMagicSceneGen4SelectShdMode);
            consumed = true;
        } else if(event.event == SubmenuIndexSetDirectWriteBlock0Mode) {
            scene_manager_next_scene(
                instance->scene_manager, NfcMagicSceneGen4SetDirectWriteBlock0Mode);
            consumed = true;
        }

        scene_manager_set_scene_state(instance->scene_manager, NfcMagicSceneGen4Menu, event.event);
    } else if(event.type == SceneManagerEventTypeBack) {
        if(instance->gen4_password.value != 0) {
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
