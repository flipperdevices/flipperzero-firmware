#include "../nfc_i.h"
#include <dolphin/dolphin.h>

typedef enum {
    NfcSceneMfUlReadStateIdle,
    NfcSceneMfUlReadStateDetecting,
    NfcSceneMfUlReadStateReading,
} NfcSceneMfUlReadState;

bool nfc_scene_mf_ultralight_read_auth_info_worker_callback(NfcWorkerEvent event, void* context) {
    Nfc* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
    return true;
}

void nfc_scene_mf_ultralight_read_auth_info_set_state(Nfc* nfc, NfcSceneMfUlReadState state) {
    uint32_t curr_state =
        scene_manager_get_scene_state(nfc->scene_manager, NfcSceneMfUltralightReadAuthInfo);
    if(curr_state != state) {
        if(state == NfcSceneMfUlReadStateDetecting) {
            popup_set_header(
                nfc->popup, "Detecting\nUltralight card", 90, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 5, 7, &I_NFC_manual);
        } else if(state == NfcSceneMfUlReadStateReading) {
            popup_set_header(
                nfc->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
            popup_set_icon(nfc->popup, 19, 23, &A_Loading_24);
        }
        scene_manager_set_scene_state(nfc->scene_manager, NfcSceneMfUltralightReadAuthInfo, state);
    }
}

void nfc_scene_mf_ultralight_read_auth_info_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcRead);

    nfc_device_clear(nfc->dev);
    // Setup view
    nfc_scene_mf_ultralight_read_auth_info_set_state(nfc, NfcSceneMfUlReadStateDetecting);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);
    // Start worker
    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateReadMfUltralightAuthInfo,
        &nfc->dev->dev_data,
        nfc_scene_mf_ultralight_read_auth_info_worker_callback,
        nfc);

    nfc_blink_start(nfc);
}

bool nfc_scene_mf_ultralight_read_auth_info_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcWorkerEventSuccess) {
            nfc_worker_stop(nfc->worker);
            MfUltralightData data = nfc->dev->dev_data.mf_ul_data;
            if(data.has_auth && data.auth_readable && data.curr_authlim == 0) {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightAuth);
            } else {
                scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightAuthWarning);
            }

            notification_message(nfc->notifications, &sequence_success);
            consumed = true;
        } else if(event.event == NfcWorkerEventCardDetected) {
            nfc_scene_mf_ultralight_read_auth_info_set_state(nfc, NfcSceneMfUlReadStateReading);
            consumed = true;
        } else if(event.event == NfcWorkerEventNoCardDetected) {
            nfc_scene_mf_ultralight_read_auth_info_set_state(nfc, NfcSceneMfUlReadStateDetecting);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_mf_ultralight_read_auth_info_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);
    // Clear view
    popup_reset(nfc->popup);
    scene_manager_set_scene_state(
        nfc->scene_manager, NfcSceneMfUltralightReadAuthInfo, NfcSceneMfUlReadStateIdle);

    nfc_blink_stop(nfc);
}
