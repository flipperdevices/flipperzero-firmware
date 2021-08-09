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

    subghz_transmitter_set_callback(
        subghz_transmitter, subghz_scene_transmitter_callback, subghz);

    subghz_transmitter_set_protocol(subghz_transmitter, subghz->protocol_result);
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTransmitter);
}

const bool subghz_scene_transmitter_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubghzTransmitterEventSend) {
            //scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSaveName);
            //return true;
        } else if(event.event == SubghzTransmitterEventBack) {
            scene_manager_previous_scene(subghz->scene_manager);
            return true;
        }
    }
    return false;
}

const void subghz_scene_transmitter_on_exit(void* context) {
    // SubGhz* subghz = context;
}
