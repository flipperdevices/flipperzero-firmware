#include "../nfc_i.h"
#include <dolphin/dolphin.h>

bool nfc_read_worker_callback(NfcWorkerEvent event, void* context) {
    Nfc* nfc = context;
    bool consumed = false;
    if(event == NfcWorkerEventReadMfClassicLoadKeyCache) {
        consumed = nfc_device_load_key_cache(nfc->dev);
    } else {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
        consumed = true;
    }
    return consumed;
}

void nfc_scene_read_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcRead);

    // Setup view
    Popup* popup = nfc->popup;
    popup_set_header(popup, "Detecting\nNFC card", 70, 34, AlignLeft, AlignTop);
    popup_set_icon(popup, 0, 3, &I_RFIDDolphinReceive_97x61);

    // Start worker
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    nfc_worker_start(
        nfc->worker, NfcWorkerStateRead, &nfc->dev->dev_data, nfc_read_worker_callback, nfc);

    nfc_blink_start(nfc);
}

bool nfc_scene_read_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventReadUidNfcB || event.event == NfcWorkerEventReadUidNfcF ||
           event.event == NfcWorkerEventReadUidNfcV) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadCardSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfUltralight) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfClassicDone) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfDesfire) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadBankCard) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneEmvReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfClassicDictAttackRequired) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicDictAttack);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_read_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
