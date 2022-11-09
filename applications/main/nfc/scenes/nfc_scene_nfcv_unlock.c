#include "../nfc_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    NfcSceneNfcVUnlockStateIdle,
    NfcSceneNfcVUnlockStateDetecting,
    NfcSceneNfcVUnlockStateUnlocked,
    NfcSceneNfcVUnlockStateAlreadyUnlocked,
    NfcSceneNfcVUnlockStateNotSupportedCard,
} NfcSceneNfcVUnlockState;

bool nfc_scene_nfcv_unlock_worker_callback(NfcWorkerEvent event, void* context) {
    Nfc* nfc = context;

    if(event == NfcWorkerEventNfcVPassKey) {
        memcpy(nfc->dev->dev_data.nfcv_data.key_privacy, nfc->byte_input_store, 4);
    } else {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
    }
    return true;
}

void nfc_scene_nfcv_unlock_popup_callback(void* context) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventViewExit);
}

void nfc_scene_nfcv_unlock_set_state(Nfc* nfc, NfcSceneNfcVUnlockState state) {
    NfcVData* nfcv_data = &(nfc->dev->dev_data.nfcv_data);

    uint32_t curr_state =
        scene_manager_get_scene_state(nfc->scene_manager, NfcSceneNfcVUnlock);
    if(curr_state != state) {
        Popup* popup = nfc->popup;
        if(state == NfcSceneNfcVUnlockStateDetecting) {
            popup_reset(popup);
            popup_set_text(
                popup, "Put Tonie On\nFlipper's Back", 97, 24, AlignCenter, AlignTop);
            popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        } else if(state == NfcSceneNfcVUnlockStateUnlocked) {
            popup_reset(popup);

            notification_message(nfc->notifications, &sequence_success);

            popup_set_header(popup, "Successfully\nUnlocked!", 94, 3, AlignCenter, AlignTop);
            popup_set_icon(popup, 0, 6, &I_RFIDDolphinSuccess_108x57);
            popup_set_context(popup, nfc);
            popup_set_callback(popup, nfc_scene_nfcv_unlock_popup_callback);
            popup_set_timeout(popup, 1500);
            //popup_enable_timeout(popup);

            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
            DOLPHIN_DEED(DolphinDeedNfcReadSuccess);

        } else if(state == NfcSceneNfcVUnlockStateAlreadyUnlocked) {
            popup_reset(popup);

            popup_set_header(popup, "Already\nUnlocked!", 94, 3, AlignCenter, AlignTop);
            popup_set_icon(popup, 0, 6, &I_RFIDDolphinSuccess_108x57);
            popup_set_context(popup, nfc);
            popup_set_callback(popup, nfc_scene_nfcv_unlock_popup_callback);
            popup_set_timeout(popup, 1500);

            view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
        } else if(state == NfcSceneNfcVUnlockStateNotSupportedCard) {
            popup_reset(popup);
            popup_set_header(popup, "Wrong Type Of Card!", 64, 3, AlignCenter, AlignTop);
            popup_set_text(
                popup,
                nfcv_data->error,
                4,
                22,
                AlignLeft,
                AlignTop);
            popup_set_icon(popup, 73, 20, &I_DolphinCommon_56x48);
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneNfcVUnlock, state);
    }
}

void nfc_scene_nfcv_unlock_on_enter(void* context) {
    Nfc* nfc = context;

    nfc_device_clear(nfc->dev);
    // Setup view
    nfc_scene_nfcv_unlock_set_state(nfc, NfcSceneNfcVUnlockStateDetecting);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateNfcVUnlock,
        &nfc->dev->dev_data,
        nfc_scene_nfcv_unlock_worker_callback,
        nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_nfcv_unlock_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventCardDetected) {
            nfc_scene_nfcv_unlock_set_state(nfc, NfcSceneNfcVUnlockStateUnlocked);
            consumed = true;
        } else if(event.event == NfcWorkerEventAborted) {
            nfc_scene_nfcv_unlock_set_state(nfc, NfcSceneNfcVUnlockStateAlreadyUnlocked);
            consumed = true;
        } else if(event.event == NfcWorkerEventNoCardDetected) {
            nfc_scene_nfcv_unlock_set_state(nfc, NfcSceneNfcVUnlockStateDetecting);
            consumed = true;
        } else if(event.event == NfcWorkerEventWrongCardDetected) {
            nfc_scene_nfcv_unlock_set_state(nfc, NfcSceneNfcVUnlockStateNotSupportedCard);
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            nfc->scene_manager, NfcSceneNfcVUnlockMenu);
    }
    return consumed;
}

void nfc_scene_nfcv_unlock_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Clear view
    popup_reset(nfc->popup);
    nfc_blink_stop(nfc);
    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneNfcVUnlock, NfcSceneNfcVUnlockStateIdle);
}
