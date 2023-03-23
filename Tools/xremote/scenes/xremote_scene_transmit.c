#include "../xremote.h"
#include "../views/xremote_transmit.h"

static const NotificationSequence* xremote_notification_sequences[] = {
    &sequence_success,
    &sequence_set_only_green_255,
    &sequence_reset_green,
    &sequence_solid_yellow,
    &sequence_reset_rgb,
    &sequence_blink_start_cyan,
    &sequence_blink_start_magenta,
    &sequence_blink_stop,
};

void xremote_transmit_callback(XRemoteCustomEvent event, void* context) {
    furi_assert(context);
    XRemote* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void xremote_scene_ir_notification_message(XRemote* app, uint32_t message) {
    notification_message(app->notification, xremote_notification_sequences[message]);
}

void xremote_scene_send_ir_signal(XRemote* app, CrossRemoteItem* item) {
    InfraredSignal* signal = xremote_remote_item_get_ir_signal(item);
    UNUSED(signal);
    DOLPHIN_DEED(DolphinDeedIrSend);
    xremote_scene_ir_notification_message(app, InfraredNotificationMessageBlinkStartSend);
    

}

void xremote_scene_send_signal(void* context, CrossRemoteItem* item) {
    furi_assert(context);
    XRemote* app = context;
    CrossRemote* remote = app->cross_remote;
    
    xremote_transmit_model_set_name(app->xremote_transmit, xremote_remote_item_get_name(item));
    xremote_transmit_model_set_type(app->xremote_transmit, item->type);
    if (item->type == XRemoteRemoteItemTypeInfrared) {
        xremote_scene_send_ir_signal(app, item);
    }
    //xremote_item_transmit(item);
    
    cross_remote_set_transmitting(remote, XRemoteTransmittingStop);
    //xremote_scene_ir_notification_message(app, InfraredNotificationMessageBlinkStop);
}

void xremote_scene_transmit_run_remote(void* context) {
    furi_assert(context);
    XRemote* app = context;
    CrossRemote* remote = app->cross_remote;

    size_t item_count = cross_remtoe_get_item_count(remote);
    for(size_t i = 0; i < item_count;) {
        if (cross_remote_get_transmitting(remote) == XRemoteTransmittingIdle) {
            cross_remote_set_transmitting(remote, XRemoteTransmittingStart);
            CrossRemoteItem* item = cross_remote_get_item(remote, i);
            xremote_scene_send_signal(app, item);
            furi_thread_flags_wait(0, FuriFlagWaitAny, 2000);
            xremote_scene_ir_notification_message(app, InfraredNotificationMessageBlinkStartSend);
        } else if(cross_remote_get_transmitting(remote) == XRemoteTransmittingStop) {
            i++;
            cross_remote_set_transmitting(remote, XRemoteTransmittingIdle);
        } 
    }
    xremote_scene_ir_notification_message(app, InfraredNotificationMessageBlinkStop);

    //xremote_transmit_model_set_name(app->xremote_transmit, cross_remote_get_name(remote));
}

void xremote_scene_transmit_on_enter(void* context) {
    furi_assert(context);
    XRemote* app = context;
    xremote_transmit_set_callback(app->xremote_transmit, xremote_transmit_callback, app);
    
    view_dispatcher_switch_to_view(app->view_dispatcher, XRemoteViewIdTransmit);
    xremote_scene_transmit_run_remote(app);
}

bool xremote_scene_transmit_on_event(void* context, SceneManagerEvent event) {
    XRemote* app = context;
    UNUSED(app);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void xremote_scene_transmit_on_exit(void* context) {
    XRemote* app = context;
    UNUSED(app);
}