#include "../nfc_app_i.h"
#include <dolphin/dolphin.h>

enum {
    NfcSceneReadEventNfcaDetected = 100,
    NfcSceneReadEventNfcbDetected,
    NfcSceneReadEventMfUltralightDetected,
    NfcSceneReadEventMfDesfireDetected,
};

NfcPollerOldCommand nfc_scene_read_worker_callback(NfcPollerOldEvent event, void* context) {
    NfcApp* nfc = context;

    NfcPollerOldCommand command = NfcPollerOldCommandContinue;

    if(event == NfcPollerOldEventNfcaDetected) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcSceneReadEventNfcaDetected);
        command = NfcPollerOldCommandStop;
    } else if(event == NfcPollerOldEventNfcbDetected) {
        view_dispatcher_send_custom_event(nfc->view_dispatcher, NfcSceneReadEventNfcbDetected);
        command = NfcPollerOldCommandStop;
    } else if(event == NfcPollerOldEventMfUltralightDetected) {
        view_dispatcher_send_custom_event(
            nfc->view_dispatcher, NfcSceneReadEventMfUltralightDetected);
        command = NfcPollerOldCommandStop;
    } else if(event == NfcPollerOldEventMfDesfireDetected) {
        view_dispatcher_send_custom_event(
            nfc->view_dispatcher, NfcSceneReadEventMfDesfireDetected);
        command = NfcPollerOldCommandStop;
    }

    return command;
}

void nfc_scene_read_on_enter(void* context) {
    NfcApp* nfc = context;

    // Setup view
    popup_reset(nfc->popup);
    popup_set_text(nfc->popup, "Apply card to\nFlipper's back", 97, 24, AlignCenter, AlignTop);
    popup_set_icon(nfc->popup, 0, 8, &I_NFC_manual_60x50);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewPopup);

    nfc_poller_start(nfc->nfc_poller, nfc_scene_read_worker_callback, nfc);

    nfc_blink_read_start(nfc);
}

bool nfc_scene_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcSceneReadEventNfcaDetected) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNfcaRead);
            consumed = true;
        } else if(event.event == NfcSceneReadEventNfcbDetected) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneNotImplemented);
            consumed = true;
        } else if(event.event == NfcSceneReadEventMfUltralightDetected) {
            mf_ultralight_auth_reset(nfc->mf_ul_auth);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfUltralightRead);
            consumed = true;
        } else if(event.event == NfcSceneReadEventMfDesfireDetected) {
            scene_manager_next_scene(nfc->scene_manager, NfcSceneMfDesfireRead);
            consumed = true;
        }
    }
    return consumed;
}

void nfc_scene_read_on_exit(void* context) {
    NfcApp* nfc = context;

    nfc_poller_stop(nfc->nfc_poller);
    // Clear view
    popup_reset(nfc->popup);

    nfc_blink_stop(nfc);
}
