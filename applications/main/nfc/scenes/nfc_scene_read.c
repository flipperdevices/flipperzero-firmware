#include "../nfc_app_i.h"

#include "../helpers/protocol_support/nfc_protocol_support.h"

static NfcCommand nfc_scene_read_poller_callback(NfcGenericEvent event, void* context) {
    NfcApp* instance = context;

    const NfcCustomEvent custom_event = nfc_protocol_support_handle_read(event, context);
    view_dispatcher_send_custom_event(instance->view_dispatcher, custom_event);
    return custom_event != NfcCustomEventReadHandlerIgnore ? NfcCommandStop : NfcCommandContinue;
}

void nfc_scene_read_on_enter(void* context) {
    NfcApp* instance = context;

    popup_set_header(
        instance->popup, "Reading card\nDon't move...", 85, 24, AlignCenter, AlignTop);
    popup_set_icon(instance->popup, 12, 23, &A_Loading_24);

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewPopup);

    const NfcProtocol protocol = instance->protocols_detected[instance->protocols_detected_idx];
    instance->poller = nfc_poller_alloc(instance->nfc, protocol);
    nfc_poller_start(instance->poller, nfc_scene_read_poller_callback, instance);

    nfc_blink_detect_start(instance);
}

bool nfc_scene_read_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == NfcCustomEventReadHandlerSuccess) {
            notification_message(instance->notifications, &sequence_success);
            scene_manager_next_scene(instance->scene_manager, NfcSceneReadSuccess);
            dolphin_deed(DolphinDeedNfcReadSuccess);
            consumed = true;
        } else if(event.event == NfcCustomEventReadHandlerAltRead) {
            // TODO: Go to alternative read scene
            consumed = true;
        } else if(event.event == NfcCustomEventReadHandlerFailure) {
            if(scene_manager_has_previous_scene(instance->scene_manager, NfcSceneDetect)) {
                scene_manager_search_and_switch_to_previous_scene(
                    instance->scene_manager, NfcSceneDetect);
            }
            consumed = true;
        }
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

    nfc_poller_stop(instance->poller);
    nfc_poller_free(instance->poller);
    popup_reset(instance->popup);

    nfc_blink_stop(instance);
}
