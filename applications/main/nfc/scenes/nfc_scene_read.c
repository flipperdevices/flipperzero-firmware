#include "../nfc_app_i.h"

static NfcCommand nfc_scene_read_poller_callback(NfcPollerEvent event, void* context) {
    UNUSED(context);
    UNUSED(event);
    // NfcApp* instance = context;

    NfcCommand command = NfcCommandContinue;

    // Process event

    // Set nfc_dev data
    // const NfcPoller* poller = event.poller;
    // const NfcProtocolType protocol_type = event.protocol_type;
    //
    // nfc_dev_set_protocol_data(instance->nfc_dev, protocol_type, nfc_pollers_api[protocol_type]->get_data(poller));

    return command;
}

void nfc_scene_read_on_enter(void* context) {
    NfcApp* instance = context;

    popup_set_header(
        instance->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
    popup_set_icon(instance->popup, 12, 23, &A_Loading_24);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    const NfcProtocolType protocol_type =
        instance->protocols_detected[instance->protocols_detected_idx];
    nfc_poller_manager_start(
        instance->poller_manager, protocol_type, nfc_scene_read_poller_callback, instance);

    nfc_blink_detect_start(instance);
}

bool nfc_scene_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        // if(event.event == NfcPollerHandlerEventReadSuccess) {
        //     notification_message(instance->notifications, &sequence_success);
        //     scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
        //     dolphin_deed(DolphinDeedNfcReadSuccess);
        //     consumed = true;
        // } else if(event.event == NfcPollerHandlerEventReadAltMethod) {
        //     // TODO: Go to alternative read scene
        //     consumed = true;
        // } else if(event.event == NfcPollerHandlerEventReadFailure) {
        //     if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneDetect)) {
        //         scene_manager_search_and_switch_to_previous_scene(instance->scene_manager, NfcSceneDetect);
        //     }
        //     consumed = true;
        // }
    } else if(event.type == SceneManagerEventTypeBack) {
        static const uint32_t possible_scenes[] = {NfcSceneSelectProtocol, NfcSceneStart};
        scene_manager_search_and_switch_to_previous_scene_one_of(
            instance->scene_manager, possible_scenes, COUNT_OF(possible_scenes));
        consumed = true;
    }

    return consumed;
}

void nfc_scene_read_on_exit(void* context) {
    NfcApp* instance = context;

    nfc_poller_manager_stop(instance->poller_manager);
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
