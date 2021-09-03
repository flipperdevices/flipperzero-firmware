#include "../subghz_i.h"
#include "../views/subghz_receiver.h"

void subghz_scene_receiver_config_callback(SubghzReceverEvent event, void* context) {
    furi_assert(context);
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, event);
}

const void subghz_scene_receiver_config_on_enter(void* context) {
    SubGhz* subghz = context;
    DialogEx* dialog_ex = subghz->dialog_ex;
    SubghzReceiver* subghz_receiver = subghz->subghz_receiver;
    dialog_ex_set_header(dialog_ex, "Config...", 63, 46, AlignLeft, AlignCenter);

    // subghz->state_notifications = NOTIFICATION_RX_STATE;
    //view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewReceiverConfig);
}

const bool subghz_scene_receiver_config_on_event(void* context, SceneManagerEvent event) {
    //SubGhz* subghz = context;

    return false;
}

const void subghz_scene_receiver_config_on_exit(void* context) {
    SubGhz* subghz = context;
        DialogEx* dialog_ex = subghz->dialog_ex;
    dialog_ex_set_header(dialog_ex, NULL, 0, 0, AlignCenter, AlignCenter);
}
