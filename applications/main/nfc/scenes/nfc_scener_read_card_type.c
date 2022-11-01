#include "../nfc_i.h"

enum SubmenuIndex {
    SubmenuIndexReadMifareClassic,
    SubmenuIndexReadMifareDesfire,
    SubmenuIndexReadMfUltralight,
    SubmenuIndexReadEMV,
};

void nfc_scene_read_card_type_submenu_callback(void* context, uint32_t index) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_read_card_type_on_enter(void* context) {
    Nfc* nfc = context;
    Submenu* submenu = nfc->submenu;

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
        "Read NTAG/Ultralight",
        SubmenuIndexReadMfUltralight,
        nfc_scene_read_card_type_submenu_callback,
        nfc);
    submenu_add_item(
        submenu,
        "Read EMV card",
        SubmenuIndexReadEMV,
        nfc_scene_read_card_type_submenu_callback,
        nfc);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_read_card_type_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexReadMifareClassic) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMfClassic);
            consumed = true;
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneReadCardType, event.event);
    }
    return consumed;
}

void nfc_scene_read_card_type_on_exit(void* context) {
    Nfc* nfc = context;

    submenu_reset(nfc->submenu);
}
