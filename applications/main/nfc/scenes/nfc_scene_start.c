#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexRead,
    SubmenuIndexDetectReader,
    SubmenuIndexSaved,
    SubmenuIndexExtraAction,
    SubmenuIndexAddManually,
    SubmenuIndexDebug,
};

void nfc_scene_start_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_start_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(submenu, "Read", SubmenuIndexRead, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Detect Reader", SubmenuIndexDetectReader, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(submenu, "Saved", SubmenuIndexSaved, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Extra Actions", SubmenuIndexExtraAction, nfc_scene_start_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Add Manually", SubmenuIndexAddManually, nfc_scene_start_submenu_callback, nfc);

    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagDebug)) {
        submenu_add_item(
            submenu, "Debug", SubmenuIndexDebug, nfc_scene_start_submenu_callback, nfc);
    }

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneStart));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_start_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuIndexRead) {
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneStart, SubmenuIndexRead);
            // TEST
            uint8_t uid[7] = {0x44, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
            uint8_t atqa[2] = {0x44, 0x00};
            nfc->nfc_dev_data.nfca_data.sak = 0x00;
            nfc->nfc_dev_data.nfca_data.uid_len = 7;
            memcpy(nfc->nfc_dev_data.nfca_data.uid, uid, 7);
            memcpy(nfc->nfc_dev_data.nfca_data.atqa, atqa, 2);

            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaEmulate);
            DOLPHIN_DEED(DolphinDeedNfcRead);
            consumed = true;
        } else if(event.event == SubmenuIndexDetectReader) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneStart, SubmenuIndexDetectReader);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexSaved) {
            // Save the scene state explicitly in each branch, so that
            // if the user cancels loading a file, the Saved menu item
            // is properly reselected.
            scene_manager_set_scene_state(nfc->scene_manager, NfcSceneStart, SubmenuIndexSaved);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexExtraAction) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneStart, SubmenuIndexExtraAction);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneExtraActions);
            consumed = true;
        } else if(event.event == SubmenuIndexAddManually) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneStart, NfcSceneNotImplemented);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexDebug) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneStart, NfcSceneNotImplemented);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_start_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
