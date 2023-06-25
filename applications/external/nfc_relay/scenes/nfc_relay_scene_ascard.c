#include "../nfc_relay.h"
#include "../as_card_worker.h"

#define TAG "NFC_RELAY_SCENE_ASCARD"

enum {
    NfcRelaySceneAsCardStateWaitPong,
    NfcRelaySceneAsCardStateWaitNfcDevData,
    NfcRelaySceneAsCardStateWaitApduReq,
    NfcRelaySceneAsCardStateWaitApduResp,
};

bool as_card_worker_callback(AsCardWorkerState state, void* context) {
    furi_assert(context);

    NfcRelay* nfc_relay = context;
    view_dispatcher_send_custom_event(nfc_relay->view_dispatcher, state);
    return true;
}

static void nfc_relay_scene_ascard_setup_view(NfcRelay* nfc_relay) {
    Popup* popup = nfc_relay->popup;
    popup_reset(popup);
    uint32_t state = scene_manager_get_scene_state(nfc_relay->scene_manager, NfcRelaySceneAsCard);

    if(state == NfcRelaySceneAsCardStateWaitPong) {
        popup_set_icon(popup, 12, 23, &I_Loading_24);
        popup_set_header(popup, "Wait Pong\nFrom\nAnother End", 52, 32, AlignLeft, AlignCenter);
    } else if(state == NfcRelaySceneAsCardStateWaitNfcDevData) {
        popup_set_icon(popup, 0, 8, &I_Loading_24);
        popup_set_text(
            popup, "Wait Card Data\nFrom\nAnother End", 128, 32, AlignRight, AlignCenter);
    } else if(state == NfcRelaySceneAsCardStateWaitApduReq) {
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(popup, "Wait Req", 128, 32, AlignRight, AlignCenter);
    } else if(state == NfcRelaySceneAsCardStateWaitApduResp) {
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(popup, "Wait Resp", 128, 32, AlignRight, AlignCenter);
    } else {
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(popup, "???", 128, 32, AlignRight, AlignCenter);
    }

    view_dispatcher_switch_to_view(nfc_relay->view_dispatcher, NfcRelayViewPopup);
}

void nfc_relay_scene_ascard_on_enter(void* context) {
    NfcRelay* nfc_relay = context;
    scene_manager_set_scene_state(
        nfc_relay->scene_manager, NfcRelaySceneAsCard, NfcRelaySceneAsCardStateWaitPong);
    nfc_relay_scene_ascard_setup_view(nfc_relay);

    // Setup and start workerr
    nfc_relay->as_card_worker = as_card_worker_alloc(nfc_relay);
    as_card_worker_start(nfc_relay->as_card_worker, as_card_worker_callback);
}

bool nfc_relay_scene_ascard_on_event(void* context, SceneManagerEvent event) {
    NfcRelay* nfc_relay = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == AsCardWorkerStateWaitPong) {
            FURI_LOG_D(TAG, "AsCardWorkerStateWaitPong");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager, NfcRelaySceneAsCard, NfcRelaySceneAsCardStateWaitPong);
            nfc_relay_scene_ascard_setup_view(nfc_relay);
            consumed = true;
        } else if(event.event == AsCardWorkerStateWaitNfcDevData) {
            FURI_LOG_D(TAG, "AsCardWorkerStateWaitNfcDevData");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsCard,
                NfcRelaySceneAsCardStateWaitNfcDevData);
            nfc_relay_scene_ascard_setup_view(nfc_relay);
            consumed = true;

        } else if(event.event == AsCardWorkerStateWaitApduReq) {
            FURI_LOG_D(TAG, "AsCardWorkerStateWaitApduReq");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsCard,
                NfcRelaySceneAsCardStateWaitApduReq);
            nfc_relay_scene_ascard_setup_view(nfc_relay);
            consumed = true;
        } else if(event.event == AsCardWorkerStateWaitApduResp) {
            FURI_LOG_D(TAG, "AsCardWorkerStateWaitApduResp");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsCard,
                NfcRelaySceneAsCardStateWaitApduResp);
            nfc_relay_scene_ascard_setup_view(nfc_relay);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_relay_scene_ascard_on_exit(void* context) {
    FURI_LOG_D(TAG, "on_exit");
    NfcRelay* nfc_relay = context;
    scene_manager_set_scene_state(
        nfc_relay->scene_manager, NfcRelaySceneAsCard, NfcRelaySceneAsCardStateWaitPong);
    as_card_worker_stop(nfc_relay->as_card_worker);
    as_card_worker_free(nfc_relay->as_card_worker);
    nfc_relay->as_card_worker = NULL;
    // Clear view
    popup_reset(nfc_relay->popup);
}