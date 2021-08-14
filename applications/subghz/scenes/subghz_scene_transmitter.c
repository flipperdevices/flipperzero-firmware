#include "../subghz_i.h"
#include "../views/subghz_transmitter.h"


void subghz_scene_transmitter_callback(SubghzTransmitterEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

const void subghz_scene_transmitter_on_enter(void* context) {
    SubGhz* subghz = context;
    SubghzTransmitter* subghz_transmitter = subghz->subghz_transmitter;

    subghz_transmitter_set_callback(subghz_transmitter, subghz_scene_transmitter_callback, subghz);

    subghz_transmitter_set_protocol(subghz_transmitter, subghz->protocol_result);
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTransmitter);
}

const bool subghz_scene_transmitter_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubghzTransmitterEventSendStart) {
            subghz_transmitter_tx_start(subghz);
            return true;
        } else if(event.event == SubghzTransmitterEventSendStop) {
            subghz_transmitter_tx_stop(subghz);
            return true;
        } else if(event.event == SubghzTransmitterEventBack) {
            scene_manager_search_and_switch_to_previous_scene(
                subghz->scene_manager, SubGhzSceneStart);
            return true;
        }
    }
    return false;
}

const void subghz_scene_transmitter_on_exit(void* context) {
    SubGhz* subghz = context;
    SubghzTransmitter* subghz_transmitter = subghz->subghz_transmitter;

    subghz_transmitter_set_callback(subghz_transmitter, NULL, subghz);
}
