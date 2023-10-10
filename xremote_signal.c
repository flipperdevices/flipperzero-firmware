/*!
 *  @file flipper-xremote/xremote_signal.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Implementation of infrared signal receiver functionality 
 */

#include "xremote_signal.h"

struct XRemoteSignalReceiver {
    XRemoteClearCallback on_clear;
    XRemoteRxCallback rx_callback;

    NotificationApp* notifications;
    InfraredWorker* worker;
    InfraredSignal* signal;

    void* context;
    bool started;
};

static void xremote_signal_receiver_rx_callback(void* context, InfraredWorkerSignal* ir_signal) {
    furi_assert(context);
    XRemoteSignalReceiver* rx_ctx = context;
    xremote_app_notification_blink(rx_ctx->notifications);

    if(infrared_worker_signal_is_decoded(ir_signal)) {
        const InfraredMessage* message;
        message = infrared_worker_get_decoded_signal(ir_signal);
        infrared_signal_set_message(rx_ctx->signal, message);
    } else {
        const uint32_t* timings;
        size_t timings_size = 0;

        infrared_worker_get_raw_signal(ir_signal, &timings, &timings_size);
        infrared_signal_set_raw_signal(
            rx_ctx->signal,
            timings,
            timings_size,
            INFRARED_COMMON_CARRIER_FREQUENCY,
            INFRARED_COMMON_DUTY_CYCLE);
    }

    if(rx_ctx->rx_callback != NULL) rx_ctx->rx_callback(rx_ctx->context, rx_ctx->signal);
}

static void xremote_signal_receiver_clear_context(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void(rx_ctx);
    xremote_app_assert_void(rx_ctx->context);
    xremote_app_assert_void(rx_ctx->on_clear);
    rx_ctx->on_clear(rx_ctx->context);
    rx_ctx->context = NULL;
}

XRemoteSignalReceiver* xremote_signal_receiver_alloc(XRemoteAppContext* app_ctx) {
    XRemoteSignalReceiver* rx_ctx = malloc(sizeof(XRemoteSignalReceiver));
    rx_ctx->signal = infrared_signal_alloc();
    rx_ctx->worker = infrared_worker_alloc();

    rx_ctx->notifications = app_ctx->notifications;
    rx_ctx->rx_callback = NULL;
    rx_ctx->on_clear = NULL;
    rx_ctx->context = NULL;
    rx_ctx->started = false;
    return rx_ctx;
}

void xremote_signal_receiver_free(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void(rx_ctx);
    xremote_signal_receiver_stop(rx_ctx);
    infrared_worker_free(rx_ctx->worker);
    infrared_signal_free(rx_ctx->signal);
    xremote_signal_receiver_clear_context(rx_ctx);
    free(rx_ctx);
}

void xremote_signal_receiver_set_context(
    XRemoteSignalReceiver* rx_ctx,
    void* context,
    XRemoteClearCallback on_clear) {
    xremote_signal_receiver_clear_context(rx_ctx);
    rx_ctx->on_clear = on_clear;
    rx_ctx->context = context;
}

void xremote_signal_receiver_set_rx_callback(
    XRemoteSignalReceiver* rx_ctx,
    XRemoteRxCallback rx_callback) {
    xremote_app_assert_void(rx_ctx);
    rx_ctx->rx_callback = rx_callback;
}

void xremote_signal_receiver_attach(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void((rx_ctx && rx_ctx->worker));
    infrared_worker_rx_set_received_signal_callback(
        rx_ctx->worker, xremote_signal_receiver_rx_callback, rx_ctx);
}

void xremote_signal_receiver_detach(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void((rx_ctx && rx_ctx->worker));
    infrared_worker_rx_set_received_signal_callback(rx_ctx->worker, NULL, NULL);
}

void xremote_signal_receiver_start(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void((rx_ctx && !rx_ctx->started));
    xremote_signal_receiver_attach(rx_ctx);
    infrared_worker_rx_start(rx_ctx->worker);
    xremote_app_notification_blink(rx_ctx->notifications);
    rx_ctx->started = true;
}

void xremote_signal_receiver_stop(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert_void((rx_ctx && rx_ctx->started));
    xremote_signal_receiver_detach(rx_ctx);
    infrared_worker_rx_stop(rx_ctx->worker);
    rx_ctx->started = false;
}

InfraredSignal* xremote_signal_receiver_get_signal(XRemoteSignalReceiver* rx_ctx) {
    xremote_app_assert(rx_ctx, NULL);
    return rx_ctx->signal;
}