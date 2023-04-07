#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    NfcSceneReadStateIdle,
    NfcSceneReadStateDetecting,
    NfcSceneReadStateReading,
} NfcSceneReadState;

void nfc_scene_read_worker_callback(NfcaPollerEvent event, void* context) {
    NfcApp* nfc = context;

    if(event.type == NfcaPollerEventTypeActivated) {
        nfca_poller_stop(nfc->nfca_poller);
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcWorkerEventReadUidNfcA);
    }

}

void nfc_scene_read_set_state(NfcApp* nfc, NfcSceneReadState state) {
    uint32_t curr_state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneRead);
    if(curr_state != state) {
        if(state == NfcSceneReadStateDetecting) {
            popup_reset(nfc->popup);
            popup_set_text(
                nfc->popup, "Apply card to\nFlipper's back", 97, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 0, 8, &I_NFC_manual_60x50);
        } else if(state == NfcSceneReadStateReading) {
            popup_reset(nfc->popup);
            popup_set_header(
                nfc->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 12, 23, &A_Loading_24);
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneRead, state);
    }
}

void nfc_scene_read_on_enter(void* context) {
    NfcApp* nfc = context;

    nfc_device_clear(nfc->dev);
    // Setup view
    nfc_scene_read_set_state(nfc, NfcSceneReadStateDetecting);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    
    nfc->nfca_poller = nfca_poller_alloc();
    nfca_poller_start(nfc->nfca_poller, nfc_scene_read_worker_callback, nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if((event.event == NfcWorkerEventReadUidNfcB) ||
           (event.event == NfcWorkerEventReadUidNfcF) ||
           (event.event == NfcWorkerEventReadUidNfcV)) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneReadCardSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadUidNfcA) {
            notification_message(nfc->notifications, &sequence_success);
            nfca_poller_get_data(nfc->nfca_poller, &nfc->nfca_data);
            nfca_poller_free(nfc->nfca_poller);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfUltralight) {
            notification_message(nfc->notifications, &sequence_success);
            // Set unlock password input to 0xFFFFFFFF only on fresh read
            memset(nfc->byte_input_store, 0xFF, sizeof(nfc->byte_input_store));
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfClassicDone) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfDesfire) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadBankCard) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneEmvReadSuccess);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventReadMfClassicDictAttackRequired) {
            if(mf_classic_dict_check_presence(MfClassicDictTypeSystem)) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfClassicDictAttack);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneDictNotFound);
            }
            consumed = true;
        } else if(event.event == NfcWorkerEventCardDetected) {
            nfc_scene_read_set_state(nfc, NfcSceneReadStateReading);
            nfc_blink_detect_start(nfc);
            consumed = true;
        } else if(event.event == NfcWorkerEventNoCardDetected) {
            nfc_scene_read_set_state(nfc, NfcSceneReadStateDetecting);
            nfc_blink_read_start(nfc);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_read_on_exit(void* context) {
    NfcApp* nfc = context;

    // Clear view
    popup_reset(nfc->popup);
    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneRead, NfcSceneReadStateIdle);

    nfc_blink_stop(nfc);
}
