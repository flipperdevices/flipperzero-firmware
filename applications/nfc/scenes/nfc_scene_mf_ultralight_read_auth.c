#include "../nfc_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    NfcSceneMfUlReadStateIdle,
    NfcSceneMfUlReadStateDetecting,
    NfcSceneMfUlReadStateReading,
} NfcSceneMfUlReadState;

bool nfc_scene_mf_ultralight_read_auth_worker_callback(NfcWorkerEvent event, void* context) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
    return true;
}

void nfc_scene_mf_ultralight_read_auth_set_state(Nfc* nfc, NfcSceneMfUlReadState state) {
    uint32_t curr_state = scene_manager_get_scene_state(nfc->scene_manager, NfcSceneRead);
    if(curr_state != state) {
        if(state == NfcSceneMfUlReadStateDetecting) {
            popup_reset(nfc->popup);
            popup_set_text(
                nfc->popup, "Apply card to\nFlipper's back", 97, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 0, 8, &I_NFC_manual);
        } else if(state == NfcSceneMfUlReadStateReading) {
            popup_reset(nfc->popup);
            popup_set_header(
                nfc->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 12, 23, &A_Loading_24);
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneRead, state);
    }
}

void nfc_scene_mf_ultralight_read_auth_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcRead);

    nfc_device_clear(nfc->dev);
    // Setup view
    nfc_scene_mf_ultralight_read_auth_set_state(nfc, NfcSceneMfUlReadStateDetecting);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateReadMfUltralightReadAuth,
        &nfc->dev->dev_data,
        nfc_scene_mf_ultralight_read_auth_worker_callback,
        nfc);

    nfc_blink_start(nfc);
}

bool nfc_scene_mf_ultralight_read_auth_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventSuccess) {
            notification_message(nfc->notifications, &sequence_success);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightReadSuccess);
            consumed = true;
        } else if(event.event == NfcWorkerEventCardDetected) {
            nfc_scene_mf_ultralight_read_auth_set_state(nfc, NfcSceneMfUlReadStateReading);
            consumed = true;
        } else if(event.event == NfcWorkerEventNoCardDetected) {
            nfc_scene_mf_ultralight_read_auth_set_state(nfc, NfcSceneMfUlReadStateDetecting);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            nfc->scene_manager, NfcSceneMfUltralightUnlockMenu);
    }
    return consumed;
}

void nfc_scene_mf_ultralight_read_auth_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Clear view
    popup_reset(nfc->popup);
    nfc_blink_stop(nfc);
}
