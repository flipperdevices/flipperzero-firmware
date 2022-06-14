#include "../nfc_i.h"

enum SubmenuIndex {
    SubmenuIndexGeneric,
    SubmenuIndexBankCard,
    SubmenuIndexMfUltralight,
    SubmenuIndexMfClassic,
    SubmenuIndexMfDesfire,
};

void nfc_scene_scripts_menu_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_scripts_menu_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu,
        "Read Generic NFC",
        SubmenuIndexGeneric,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Bank Card",
        SubmenuIndexBankCard,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare Ultral/Ntag",
        SubmenuIndexMfUltralight,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare Classic",
        SubmenuIndexMfClassic,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare DESFire",
        SubmenuIndexMfDesfire,
        nfc_scene_scripts_menu_submenu_callback,
        nfc);
    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneScriptsMenu));
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_scripts_menu_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexGeneric) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneScriptsMenu, SubmenuIndexGeneric);
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneReadCard, 0); // Don't auto-run reading scripts
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadCard);
            consumed = true;
        } else if(event.event == SubmenuIndexBankCard) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneScriptsMenu, SubmenuIndexBankCard);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadEmvApp);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUltralight) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneScriptsMenu, SubmenuIndexMfUltralight);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareUl);
            consumed = true;
        } else if(event.event == SubmenuIndexMfClassic) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneScriptsMenu, SubmenuIndexMfClassic);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareClassic);
            consumed = true;
        } else if(event.event == SubmenuIndexMfDesfire) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneScriptsMenu, SubmenuIndexMfDesfire);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareDesfire);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_scripts_menu_on_exit(void* context) {
    Nfc* nfc = context;
    submenu_reset(nfc->submenu);
}
