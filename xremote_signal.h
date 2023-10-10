/*!
 *  @file flipper-xremote/xremote_signal.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Implementation of infrared signal receiver functionality 
 */

#pragma once

#include "xremote_app.h"
#include "infrared/infrared_signal.h"

typedef void (*XRemoteRxCallback)(void* context, InfraredSignal* signal);
typedef struct XRemoteSignalReceiver XRemoteSignalReceiver;

XRemoteSignalReceiver* xremote_signal_receiver_alloc(XRemoteAppContext* app_ctx);
void xremote_signal_receiver_free(XRemoteSignalReceiver* rx_ctx);

void xremote_signal_receiver_set_context(
    XRemoteSignalReceiver* rx_ctx,
    void* context,
    XRemoteClearCallback on_clear);
void xremote_signal_receiver_set_rx_callback(
    XRemoteSignalReceiver* rx_ctx,
    XRemoteRxCallback rx_callback);
InfraredSignal* xremote_signal_receiver_get_signal(XRemoteSignalReceiver* rx_ctx);

void xremote_signal_receiver_start(XRemoteSignalReceiver* rx_ctx);
void xremote_signal_receiver_stop(XRemoteSignalReceiver* rx_ctx);

void xremote_signal_receiver_detach(XRemoteSignalReceiver* rx_ctx);
void xremote_signal_receiver_attach(XRemoteSignalReceiver* rx_ctx);
