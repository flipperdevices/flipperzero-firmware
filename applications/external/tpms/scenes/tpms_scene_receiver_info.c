#include "../tpms_app_i.h"
#include "../views/tpms_receiver.h"

void tpms_scene_receiver_info_callback(TPMSCustomEvent event, void* context) {
    furi_assert(context);
    TPMSApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

static void tpms_scene_receiver_info_add_to_history_callback(
    SubGhzReceiver* receiver,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context) {
    furi_assert(context);
    TPMSApp* app = context;

    if(tpms_history_add_to_history(app->txrx->history, decoder_base, app->txrx->preset) ==
       TPMSHistoryStateAddKeyUpdateData) {
        tpms_view_receiver_info_update(
            app->tpms_receiver_info,
            tpms_history_get_raw_data(app->txrx->history, app->txrx->idx_menu_chosen));
        subghz_receiver_reset(receiver);

        notification_message(app->notifications, &sequence_blink_green_10);
        app->txrx->rx_key_state = TPMSRxKeyStateAddKey;
    }
}

void tpms_scene_receiver_info_on_enter(void* context) {
    TPMSApp* app = context;

    subghz_receiver_set_rx_callback(
        app->txrx->receiver, tpms_scene_receiver_info_add_to_history_callback, app);
    tpms_view_receiver_info_update(
        app->tpms_receiver_info,
        tpms_history_get_raw_data(app->txrx->history, app->txrx->idx_menu_chosen));
    view_dispatcher_switch_to_view(app->view_dispatcher, TPMSViewReceiverInfo);
}

bool tpms_scene_receiver_info_on_event(void* context, SceneManagerEvent event) {
    TPMSApp* app = context;
    bool consumed = false;
    UNUSED(app);
    UNUSED(event);
    return consumed;
}

void tpms_scene_receiver_info_on_exit(void* context) {
    UNUSED(context);
}
