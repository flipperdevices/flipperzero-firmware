#include "../nfc_app_i.h"

enum SubmenuIndex {
    SubmenuIndexReadNFCA,
    SubmenuIndexReadMfUltralight,
    SubmenuIndexReadMifareClassic,
    SubmenuIndexReadMifareDesfire,
    SubmenuIndexReadEMV,
};

void nfc_scene_read_card_type_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_read_card_type_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu,
        "Read NFC-A data",
        SubmenuIndexReadNFCA,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read NTAG/Ultralight",
        SubmenuIndexReadMfUltralight,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare Classic",
        SubmenuIndexReadMifareClassic,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read Mifare DESFire",
        SubmenuIndexReadMifareDesfire,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read EMV card",
        SubmenuIndexReadEMV,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    uint32_t state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneReadCardType);
    submenu_set_selected_item(submenu, state);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_read_card_type_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexReadMifareDesfire) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        }
        if(event.event == SubmenuIndexReadMfUltralight) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightRead);
            consumed = true;
        }
        if(event.event == SubmenuIndexReadMifareClassic) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicReadSupportedCard);
            consumed = true;
        }
        if(event.event == SubmenuIndexReadEMV) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        }
        if(event.event == SubmenuIndexReadNFCA) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaRead);
            consumed = true;
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneReadCardType, event.event);
    }
    return consumed;
}

void nfc_scene_read_card_type_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
