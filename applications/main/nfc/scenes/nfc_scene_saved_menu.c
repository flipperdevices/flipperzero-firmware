#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum SubmenuIndex {
    SubmenuIndexEmulate,
    SubmenuIndexEditUid,
    SubmenuIndexDetectReader,
    SubmenuIndexWrite,
    SubmenuIndexUpdate,
    SubmenuIndexRename,
    SubmenuIndexDelete,
    SubmenuIndexInfo,
    SubmenuIndexRestoreOriginal,
    SubmenuIndexMfUlUnlockByReader,
    SubmenuIndexMfUlUnlockByPassword,
};

void nfc_scene_saved_menu_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_saved_menu_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    NfcDevData* data = &nfc->nfc_dev_data;

    if(data->protocol == NfcDevProtocolNfca) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexEmulate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu, "Edit UID", SubmenuIndexEditUid, nfc_scene_saved_menu_submenu_callback, nfc);
    }

    if(data->protocol == NfcDevProtocolMfDesfire) {
        submenu_add_item(
            submenu,
            "Emulate UID",
            SubmenuIndexEmulate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
    }

    if((data->protocol == NfcDevProtocolMfUltralight) ||
       (data->protocol == NfcDevProtocolMfClassic)) {
        submenu_add_item(
            submenu, "Emulate", SubmenuIndexEmulate, nfc_scene_saved_menu_submenu_callback, nfc);
    }

    if(data->protocol == NfcDevProtocolMfClassic) {
        // TODO
        // if(!mifare_classic_is_card_read(&nfc->dev->dev_data.mf_classic_data))
        submenu_add_item(
            submenu,
            "Detect Reader",
            SubmenuIndexDetectReader,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu,
            "Write to Initial Card",
            SubmenuIndexWrite,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
        submenu_add_item(
            submenu,
            "Update from Initial Card",
            SubmenuIndexUpdate,
            nfc_scene_saved_menu_submenu_callback,
            nfc);
    }

    submenu_add_item(
        submenu, "Info", SubmenuIndexInfo, nfc_scene_saved_menu_submenu_callback, nfc);

    if(data->protocol == NfcDevProtocolMfUltralight) {
        if(!mf_ultralight_is_all_data_read(&data->mf_ul_data)) {
            submenu_add_item(
                submenu,
                "Unlock with Reader",
                SubmenuIndexMfUlUnlockByReader,
                nfc_scene_saved_menu_submenu_callback,
                nfc);
            submenu_add_item(
                submenu,
                "Unlock with Password",
                SubmenuIndexMfUlUnlockByPassword,
                nfc_scene_saved_menu_submenu_callback,
                nfc);
        }
    }
    // TODO
    // if(nfc_is_shadow_file_exist)
    submenu_add_item(
        submenu,
        "Restore to original",
        SubmenuIndexRestoreOriginal,
        nfc_scene_saved_menu_submenu_callback,
        nfc);

    submenu_add_item(
        submenu, "Rename", SubmenuIndexRename, nfc_scene_saved_menu_submenu_callback, nfc);
    submenu_add_item(
        submenu, "Delete", SubmenuIndexDelete, nfc_scene_saved_menu_submenu_callback, nfc);
    submenu_set_selected_item(
        nfc->submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneSavedMenu));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_saved_menu_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;

    NfcDevData* data = &nfc->nfc_dev_data;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneSavedMenu, event.event);
        if(event.event == SubmenuIndexEmulate) {
            if(data->protocol == NfcDevProtocolMfUltralight) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightEmulate);
            } else if(data->protocol == NfcDevProtocolMfClassic) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaEmulate);
            }
            DOLPHIN_DEED(DolphinDeedNfcEmulate);
            consumed = true;
        } else if(event.event == SubmenuIndexDetectReader) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            DOLPHIN_DEED(DolphinDeedNfcDetectReader);
            consumed = true;
        } else if(event.event == SubmenuIndexWrite) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexUpdate) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexRename) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveName);
            consumed = true;
        } else if(event.event == SubmenuIndexEditUid) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneSetUid);
            consumed = true;
        } else if(event.event == SubmenuIndexDelete) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneDelete);
            consumed = true;
        } else if(event.event == SubmenuIndexInfo) {
            // bool application_info_present = false;
            // if(
            //     dev_data->protocol == NfcDeviceProtocolMifareClassic ||
            //     dev_data->protocol == NfcDeviceProtocolMifareUl) {
            //     application_info_present = nfc_supported_card_verify_and_parse(dev_data);
            // }

            // FURI_LOG_I("nfc", "application_info_present: %d", application_info_present);

            // if(application_info_present) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            // } else {
                // scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcDataInfo);
            // }
            consumed = true;
        } else if(event.event == SubmenuIndexRestoreOriginal) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUlUnlockByReader) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == SubmenuIndexMfUlUnlockByPassword) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightUnlockMenu);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_scene_saved_menu_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
