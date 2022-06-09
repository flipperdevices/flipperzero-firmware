#include "../infrared_i.h"

static void infrared_scene_learn_signal_received_callback(
    void* context,
    InfraredWorkerSignal* received_signal) {
    Infrared* infrared = context;

    if(infrared_worker_signal_is_decoded(received_signal)) {
        const uint32_t* timings;
        size_t timings_size;
        infrared_worker_get_raw_signal(received_signal, &timings, &timings_size);
        infrared_signal_set_raw_signal(
            infrared->received_signal,
            timings,
            timings_size,
            INFRARED_COMMON_CARRIER_FREQUENCY,
            INFRARED_COMMON_DUTY_CYCLE);
    } else {
        infrared_signal_set_message(
            infrared->received_signal, infrared_worker_get_decoded_signal(received_signal));
    }

    infrared_worker_rx_set_received_signal_callback(infrared->worker, NULL, NULL);
    view_dispatcher_send_custom_event(
        infrared->view_dispatcher, InfraredCustomEventSignalReceived);
}

void infrared_scene_learn_on_enter(void* context) {
    Infrared* infrared = context;
    Popup* popup = infrared->popup;
    InfraredWorker* worker = infrared->worker;

    infrared_worker_rx_set_received_signal_callback(
        worker, infrared_scene_learn_signal_received_callback, context);
    infrared_worker_rx_start(worker);

    popup_set_icon(popup, 0, 32, &I_InfraredLearnShort_128x31);
    popup_set_header(popup, NULL, 0, 0, AlignCenter, AlignCenter);
    popup_set_text(
        popup, "Point the remote at IR port\nand push the button", 5, 10, AlignLeft, AlignCenter);
    popup_set_callback(popup, NULL);

    view_dispatcher_switch_to_view(infrared->view_dispatcher, InfraredViewPopup);
}

bool infrared_scene_learn_on_event(void* context, SceneManagerEvent event) {
    Infrared* infrared = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeTick) {
        infrared_play_notification_message(infrared, InfraredNotificationMessageBlinkRead);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == InfraredCustomEventSignalReceived) {
            infrared_play_notification_message(infrared, InfraredNotificationMessageSuccess);
            scene_manager_next_scene(infrared->scene_manager, InfraredSceneLearnSuccess);
            consumed = true;
        }
    }

    return consumed;
}

void infrared_scene_learn_on_exit(void* context) {
    Infrared* infrared = context;
    Popup* popup = infrared->popup;
    infrared_worker_rx_stop(infrared->worker);
    popup_set_icon(popup, 0, 0, NULL);
    popup_set_text(popup, NULL, 0, 0, AlignCenter, AlignCenter);
}
