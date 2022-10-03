#include "../nfc_i.h"
#include <dolphin/dolphin.h>

#define NFC_SCENE_DETECT_READER_NONCES_MAX (10U)

bool nfc_detect_reader_worker_callback(NfcWorkerEvent event, void* context) {
    UNUSED(event);
    furi_assert(context);
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, event);
    return true;
}

void nfc_scene_detect_reader_callback(void* context) {
    furi_assert(context);
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcCustomEventViewExit);
}

void nfc_scene_detect_reader_on_enter(void* context) {
    Nfc* nfc = context;
    DOLPHIN_DEED(DolphinDeedNfcEmulate);

    detect_reader_set_callback(nfc->detect_reader, nfc_scene_detect_reader_callback, nfc);
    detect_reader_set_nonces_max(nfc->detect_reader, NFC_SCENE_DETECT_READER_NONCES_MAX);

    // Store number of collected nonces in scene state
    scene_manager_set_scene_state(nfc->scene_manager, NfcSceneDetectReader, 0);

    nfc_worker_start(
        nfc->worker,
        NfcWorkerStateAnalyzeReader,
        &nfc->dev->dev_data,
        nfc_detect_reader_worker_callback,
        nfc);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewDetectReader);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_detect_reader_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;
    uint32_t nonces_collected =
        scene_manager_get_scene_state(nfc->scene_manager, NfcSceneDetectReader);

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventViewExit) {
            nfc_worker_stop(nfc->worker);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfkeyNoncesInfo);
            consumed = true;
        } else if(event.event == NfcWorkerEventDetectReaderMfkeyCollected) {
            if(nonces_collected < NFC_SCENE_DETECT_READER_NONCES_MAX) {
                nonces_collected++;
                scene_manager_set_scene_state(
                    nfc->scene_manager, NfcSceneDetectReader, nonces_collected);
                detect_reader_set_nonces_collected(nfc->detect_reader, nonces_collected);
            } else {
                nfc_worker_stop(nfc->worker);
                detect_reader_set_state(nfc->detect_reader, DetectReaderStateDone);
            }
            consumed = true;
        } else if(event.event == NfcWorkerEventDetectReaderDetected) {
            detect_reader_set_state(nfc->detect_reader, DetectReaderStateReaderDetected);
        } else if(event.event == NfcWorkerEventDetectReaderLost) {
            detect_reader_set_state(nfc->detect_reader, DetectReaderStateReaderLost);
        }
    }

    return consumed;
}

void nfc_scene_detect_reader_on_exit(void* context) {
    Nfc* nfc = context;

    // Stop worker
    nfc_worker_stop(nfc->worker);

    // Clear view
    detect_reader_reset(nfc->detect_reader);

    nfc_blink_stop(nfc);
}
