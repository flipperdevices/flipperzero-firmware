#include "../tullave_i.h"
#include "../lib/tullave/protocols/tullave_iso14443_4a.c"

typedef enum {
    TuLlaveSceneDetecting,
    TuLlaveSceneReading,
    TuLlaveSceneIdle
} TuLlaveSceneReadState;

void tullave_scene_detect_stop_poller(TuLlaveApp* instance) {
    furi_assert(instance->nfc_poller);
    nfc_poller_stop(instance->nfc_poller);
    nfc_poller_free(instance->nfc_poller);
}

static void tullave_scene_detect_set_state(TuLlaveApp* instance, TuLlaveSceneReadState state) {
    Popup* popup = instance->popup;
    popup_reset(popup);

    scene_manager_set_scene_state(instance->scene_manager, TuLlaveSceneDetect, state);

    if(state == TuLlaveSceneDetecting) {
        popup_set_icon(instance->popup, 0, 8, &I_NFC_manual_60x50);
        popup_set_text(
            instance->popup, "Put TuLlave\ncard\n in the back", 128, 32, AlignRight, AlignCenter);
    } else if(state == TuLlaveSceneReading) {
        popup_set_icon(instance->popup, 12, 23, &A_Loading_24);
        popup_set_text(
            instance->popup, "Reading TuLlave\nDon't move", 128, 32, AlignRight, AlignCenter);
    }

    view_dispatcher_switch_to_view(instance->view_dispatcher, TuLlaveViewPopup);
}

bool tullave_scene_detect_on_event(void* context, SceneManagerEvent event) {
    TuLlaveApp* instance = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcPollerEventReadingData) {
            tullave_scene_detect_set_state(instance, TuLlaveSceneReading);
            consumed = true;
        } else if(event.event == NfcPollerEventSuccess) {
            tullave_scene_detect_stop_poller(instance);
            scene_manager_next_scene(instance->scene_manager, TuLlaveSceneRdSuccess);
            consumed = true;
        } else if(event.event == NfcPollerEventError) {
            tullave_scene_detect_stop_poller(instance);
            scene_manager_next_scene(instance->scene_manager, TuLlaveSceneError);
            consumed = true;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        tullave_scene_detect_stop_poller(instance);
    }
    return consumed;
}

void tullave_scene_detect_on_enter(void* context) {
    TuLlaveApp* instance = context;

    tullave_scene_detect_set_state(instance, TuLlaveSceneDetecting);

    notification_message(instance->notifications, &sequence_blink_start_blue);

    //Only ISO14443-3a is supported for TuLlave Reading
    instance->nfc_poller = nfc_poller_alloc(instance->nfc, NfcProtocolIso14443_4a);
    tullave_scene_read_on_enter_iso14443_4a(instance);
}

void tullave_scene_detect_on_exit(void* context) {
    TuLlaveApp* instance = context;
    notification_message(instance->notifications, &sequence_blink_stop);
}
