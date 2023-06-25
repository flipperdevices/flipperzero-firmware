#include "../nfc_relay.h"
#include "../as_reader_worker.h"

#define TAG "NFC_RELAY_SCENE_ASREADER"

enum {
    NfcRelaySceneAsReaderStateWaitPong,
    NfcRelaySceneAsReaderStateCardSearch,
    NfcRelaySceneAsReaderStateCardFound,
};

bool as_reader_worker_callback(AsReaderWorkerState state, void* context) {
    furi_assert(context);

    NfcRelay* nfc_relay = context;
    view_dispatcher_send_custom_event(nfc_relay->view_dispatcher, state);
    return true;
}

static void nfc_relay_scene_asreader_setup_view(NfcRelay* nfc_relay) {
    Popup* popup = nfc_relay->popup;
    popup_reset(popup);
    uint32_t state =
        scene_manager_get_scene_state(nfc_relay->scene_manager, NfcRelaySceneAsReader);
    FURI_LOG_D(TAG, "state: %ld", state);
    if(state == NfcRelaySceneAsReaderStateWaitPong) {
        popup_set_icon(popup, 12, 23, &I_Loading_24);
        popup_set_header(popup, "Wait Pong\nFrom \nAnother End", 52, 32, AlignLeft, AlignCenter);
    } else if(state == NfcRelaySceneAsReaderStateCardSearch) {
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(popup, "Apply card to\nthe back", 128, 32, AlignRight, AlignCenter);
    } else if(state == NfcRelaySceneAsReaderStateCardFound) {
        popup_set_icon(popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            popup,
            "Now interacting\nwith\nAnother End\nand Card",
            128,
            32,
            AlignRight,
            AlignCenter);
    } else {
        FURI_LOG_E(TAG, "what state?");
    }

    view_dispatcher_switch_to_view(nfc_relay->view_dispatcher, NfcRelayViewPopup);
}

void nfc_relay_scene_asreader_on_enter(void* context) {
    NfcRelay* nfc_relay = context;
    scene_manager_set_scene_state(
        nfc_relay->scene_manager, NfcRelaySceneAsReader, NfcRelaySceneAsReaderStateWaitPong);
    nfc_relay_scene_asreader_setup_view(nfc_relay);

    // Setup and start worker
    nfc_relay->as_reader_worker = as_reader_worker_alloc(nfc_relay);
    as_reader_worker_start(nfc_relay->as_reader_worker, as_reader_worker_callback);
}

bool nfc_relay_scene_asreader_on_event(void* context, SceneManagerEvent event) {
    NfcRelay* nfc_relay = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == AsReaderWorkerStateWaitPong) {
            FURI_LOG_D(TAG, "AsReaderWorkerStateWaitPong");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsReader,
                NfcRelaySceneAsReaderStateWaitPong);
            nfc_relay_scene_asreader_setup_view(nfc_relay);
            consumed = true;
        } else if(event.event == AsReaderWorkerStateCardSearch) {
            FURI_LOG_D(TAG, "AsReaderWorkerStateCardSearch");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsReader,
                NfcRelaySceneAsReaderStateCardSearch);
            nfc_relay_scene_asreader_setup_view(nfc_relay);
            consumed = true;
        } else if(event.event == AsReaderWorkerStateCardFound) {
            FURI_LOG_D(TAG, "AsReaderWorkerStateCardFound");
            scene_manager_set_scene_state(
                nfc_relay->scene_manager,
                NfcRelaySceneAsReader,
                NfcRelaySceneAsReaderStateCardFound);
            nfc_relay_scene_asreader_setup_view(nfc_relay);
            consumed = true;
        }
    }

    return consumed;
}

void nfc_relay_scene_asreader_on_exit(void* context) {
    NfcRelay* nfc_relay = context;
    scene_manager_set_scene_state(
        nfc_relay->scene_manager, NfcRelaySceneAsReader, NfcRelaySceneAsReaderStateWaitPong);
    as_reader_worker_stop(nfc_relay->as_reader_worker);
    as_reader_worker_free(nfc_relay->as_reader_worker);
    nfc_relay->as_reader_worker = NULL;
    // Clear view
    popup_reset(nfc_relay->popup);
}