#include "../nfc_app_i.h"

#define NFC_SCENE_DETECT_READER_PAIR_NONCES_MAX (10U)

static const NotificationSequence sequence_detect_reader = {
    &message_green_255,
    &message_blue_255,
    &message_do_not_reset,
    NULL,
};

void nfc_scene_mf_classic_detect_reader_view_callback(void* context) {
    NfcApp* instance = context;

    view_dispatcher_send_custom_event(instance->view_dispatcher, NfcCustomEventViewExit);
}

void nfc_scene_mf_classic_detect_reader_on_enter(void* context) {
    NfcApp* instance = context;

    if(nfc_device_get_protocol(instance->nfc_device) == NfcProtocolInvalid) {
        nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic4k_7b, instance->nfc_device);
        MfClassicData* mfc_data = nfc_device_get_data(instance->nfc_device, NfcProtocolMfClassic);
        uint8_t mfc_sectors = mf_classic_get_total_sectors_num(mfc_data->type);
        for(size_t i = 0; i < mfc_sectors; i++) {
            mf_classic_set_key_not_found(mfc_data, i, MfClassicKeyTypeA);
            mf_classic_set_key_not_found(mfc_data, i, MfClassicKeyTypeB);            
        }
    }

    detect_reader_set_nonces_max(instance->detect_reader, NFC_SCENE_DETECT_READER_PAIR_NONCES_MAX);
    detect_reader_set_callback(
        instance->detect_reader, nfc_scene_mf_classic_detect_reader_view_callback, instance);

    // Store number of collected nonces in scene state
    notification_message(instance->notifications, &sequence_detect_reader);

    instance->listener = nfc_listener_alloc(instance->nfc, );
    nfc_listener_start(instance->listener, )

    view_dispatcher_switch_to_view(instance->view_dispatcher, NfcViewDetectReader);
}

bool nfc_scene_mf_classic_detect_reader_on_event(void* context, SceneManagerEvent event) {
    NfcApp* instance = context;
    bool consumed = false;
    UNUSED(instance);
    UNUSED(event);

    // if(event.type == SceneManagerEventTypeCustom) {
    //     if(event.event == NfcCustomEventViewExit) {
    //         nfc_worker_stop(nfc->worker);
    //         scene_manager_next_scene(nfc->scene_manager, NfcSceneMfkeyNoncesInfo);
    //         consumed = true;
    //     } else if(event.event == NfcWorkerEventDetectReaderMfkeyCollected) {
    //         nonces_collected += 2;
    //         scene_manager_set_scene_state(
    //             nfc->scene_manager, NfcSceneDetectReader, nonces_collected);
    //         detect_reader_set_nonces_collected(nfc->detect_reader, nonces_collected);
    //         if(nonces_collected >= NFC_SCENE_DETECT_READER_PAIR_NONCES_MAX) {
    //             detect_reader_set_state(nfc->detect_reader, DetectReaderStateDone);
    //             nfc_blink_stop(nfc);
    //             notification_message(nfc->notifications, &sequence_single_vibro);
    //             notification_message(nfc->notifications, &sequence_set_green_255);
    //             nfc_worker_stop(nfc->worker);
    //         }
    //         consumed = true;
    //     } else if(event.event == NfcWorkerEventDetectReaderDetected) {
    //         if(nonces_collected < NFC_SCENE_DETECT_READER_PAIR_NONCES_MAX) {
    //             notification_message(nfc->notifications, &sequence_blink_start_cyan);
    //             detect_reader_set_state(nfc->detect_reader, DetectReaderStateReaderDetected);
    //         }
    //     } else if(event.event == NfcWorkerEventDetectReaderLost) {
    //         if(nonces_collected < NFC_SCENE_DETECT_READER_PAIR_NONCES_MAX) {
    //             nfc_blink_stop(nfc);
    //             notification_message(nfc->notifications, &sequence_detect_reader);
    //             detect_reader_set_state(nfc->detect_reader, DetectReaderStateReaderLost);
    //         }
    //     }
    // }

    return consumed;
}

void nfc_scene_mf_classic_detect_reader_on_exit(void* context) {
    NfcApp* instance = context;

    // Clear view
    detect_reader_reset(instance->detect_reader);

    // Stop notifications
    nfc_blink_stop(instance);
    notification_message(instance->notifications, &sequence_reset_green);
}
