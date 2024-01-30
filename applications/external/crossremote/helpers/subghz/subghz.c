/* Reduced variant of the Flipper Zero SubGhz Class */

#include "subghz_i.h"
#include "../../helpers/xremote_custom_event.h"
#include "../../helpers/xremote_led.h"

SubGhz* subghz_alloc() {
    SubGhz* subghz = malloc(sizeof(SubGhz));

    subghz->file_path = furi_string_alloc();
    subghz->txrx = subghz_txrx_alloc();

    return subghz;
}

void subghz_free(SubGhz* subghz) {
    subghz_txrx_free(subghz->txrx);
    furi_string_free(subghz->file_path);

    // The rest
    free(subghz);
}

void subghz_scene_transmit_callback_end_tx(void* context) {
    furi_assert(context);
    FURI_LOG_D(TAG, "callback end");
    XRemote* app = context;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, XRemoteCustomEventViewTransmitterSendStop);

    //app->state_notifications = SubGhzNotificationStateIDLE;
    //subghz_txrx_stop(app->subghz->txrx);
    //app->transmitting = false;
    //xremote_scene_ir_notification_message(app, SubGhzNotificationMessageBlinkStop);
    xremote_cross_remote_set_transmitting(app->cross_remote, XRemoteTransmittingStopSubghz);
}

void subghz_send(void* context, const char* path) {
    XRemote* app = context;

    subghz_load_protocol_from_file(app->subghz, path);
    FURI_LOG_D(TAG, "Starting Transmission");
    subghz_txrx_tx_start(
        app->subghz->txrx,
        subghz_txrx_get_fff_data(app->subghz->txrx)); //Seems like it must be done this way

    FURI_LOG_D(TAG, "setting sugbhz raw file encoder worker callback");
    subghz_txrx_set_raw_file_encoder_worker_callback_end(
        app->subghz->txrx, subghz_scene_transmit_callback_end_tx, app);
    app->state_notifications = SubGhzNotificationStateTx;

    FURI_LOG_D(TAG, "Finished Transmitting");
}