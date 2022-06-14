#include "../nfc_i.h"
#include <dolphin/dolphin.h>

void nfc_read_card_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventWorkerExit);
}

void nfc_scene_read_card_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcRead);

    // Setup view
    Popup* popup = nfc->popup;
    popup_set_header(popup, "Detecting\nNFC tag", 70, 34, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    // Start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc_worker_start(
        nfc->worker, NfcWorkerStateDetect, &nfc->dev->dev_data, nfc_read_card_worker_callback, nfc);

    nfc_blink_start(nfc);
}

bool nfc_scene_read_card_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    uint32_t auto_read = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneReadCard);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventWorkerExit) {
            if(auto_read) {
                NfcProtocol card_type = nfc->dev->dev_data.protocol;
                if(card_type == NfcDeviceProtocolMifareUl) {
                    scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareUl);
                } else if(card_type == NfcDeviceProtocolMifareDesfire) {
                    scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareDesfire);
                } else if(card_type == NfcDeviceProtocolEMV) {
                    scene_manager_next_scene(nfc->scene_manager, NfcSceneReadEmvApp);
                } else if(card_type == NfcDeviceProtocolMifareClassic) {
                    scene_manager_next_scene(nfc->scene_manager, NfcSceneReadMifareClassic);
                } else { // Unknown protocol type, fallback to generic.
                    scene_manager_next_scene(nfc->scene_manager, NfcSceneReadCardSuccess);
                }
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneReadCardSuccess);
            }
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeTick) {
        consumed = true;
    }
    return consumed;
}

void nfc_scene_read_card_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
