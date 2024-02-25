#include "../tullave_i.h"
#include "../lib/tullave/protocols/tullave_poller.h"

typedef enum {
    TuLlaveSceneDetecting,
    TuLlaveSceneReading,
    TuLlaveSceneIdle
} TuLlaveSceneReadState;

void tullave_scene_detect_stop_poller(TuLlaveApp* instance) {
    furi_assert(instance->tullave_poller);
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
        tullave_poller_free(instance->tullave_poller);
    }
    return consumed;
}

NfcCommand tullave_scene_detect_poller_callback(NfcGenericEvent event, void* context) {
    UNUSED(event);
    furi_assert(context);
    TuLlavePoller* poller = context;
    TuLlaveApp* instance = poller->context;
    if(poller->read_state == TuLlaveStateReadCompleted) {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcPollerEventSuccess);
    } else {
        view_dispatcher_send_custom_event(instance->view_dispatcher, NfcPollerEventError);
    }
    return NfcCommandStop;
}

void tullave_scene_detect_on_enter(void* context) {
    TuLlaveApp* instance = context;
    furi_assert(instance);

    tullave_scene_detect_set_state(instance, TuLlaveSceneDetecting);

    notification_message(instance->notifications, &sequence_blink_start_blue);

    // This initializes the poller and the card_data structure
    instance->tullave_poller = tullave_poller_alloc();
    // The context is required for the poller, so we can recover it in the callback.
    // This is useful if we need to invoke methods that are not currently supported by the poller like
    // view_dispatcher_send_custom_event
    instance->tullave_poller->context = instance;
    instance->tullave_poller->callback = tullave_scene_detect_poller_callback;

    // Only ISO14443-4a is supported for TuLlave Reading
    instance->nfc_poller = nfc_poller_alloc(instance->nfc, NfcProtocolIso14443_4a);
    nfc_poller_start(instance->nfc_poller, tullave_poller_run, instance->tullave_poller);
}

void tullave_scene_detect_on_exit(void* context) {
    TuLlaveApp* instance = context;
    notification_message(instance->notifications, &sequence_blink_stop);
}
