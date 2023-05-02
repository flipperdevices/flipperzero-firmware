#include "../nfc_app_i.h"

enum SubmenuIndex {
    SubmenuIndexGeneratorsStart,
    SubmenuIndexNFCA4 = NfcDataGeneratorTypeNum,
    SubmenuIndexNFCA7,
};

void nfc_scene_set_type_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_set_type_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;
    // Clear device name
    furi_string_reset(nfc->file_name);
    submenu_add_item(
        submenu, "NFC-A 7-bytes UID", SubmenuIndexNFCA7, nfc_scene_set_type_submenu_callback, nfc);
    submenu_add_item(
        submenu, "NFC-A 4-bytes UID", SubmenuIndexNFCA4, nfc_scene_set_type_submenu_callback, nfc);

    for(size_t i = 0; i < NfcDataGeneratorTypeNum; i++) {
        const char* name = nfc_data_generator_get_name(i);
        submenu_add_item(submenu, name, i, nfc_scene_set_type_submenu_callback, nfc);
    }

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_set_type_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexNFCA7) {
            nfc->nfc_dev_data.protocol = NfcDevProtocolNfca;
            nfc->nfc_dev_data.nfca_data.uid_len = 7;
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexNFCA4) {
            nfc->nfc_dev_data.protocol = NfcDevProtocolNfca;
            nfc->nfc_dev_data.nfca_data.uid_len = 4;
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else {
            nfc_data_generator_fill_data(event.event, &nfc->nfc_dev_data);
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneGenerateInfo, event.event);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneGenerateInfo);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_set_type_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
