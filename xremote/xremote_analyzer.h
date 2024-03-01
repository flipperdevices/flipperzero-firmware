/*!
 *  @file flipper-xremote/xremote_analyzer.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Infrared remote singnal analyzer and custom view events.
 */

#pragma once

#include "xremote_app.h"
#include "xremote_signal.h"

typedef struct XRemoteSignalAnalyzer XRemoteSignalAnalyzer;

void xremote_signal_analyzer_send_event(XRemoteSignalAnalyzer* analyzer, XRemoteEvent event);
XRemoteSignalReceiver* xremote_signal_analyzer_get_ir_receiver(XRemoteSignalAnalyzer* analyzer);
XRemoteAppContext* xremote_signal_analyzer_get_app_context(XRemoteSignalAnalyzer* analyzer);
InfraredSignal* xremote_signal_analyzer_get_ir_signal(XRemoteSignalAnalyzer* analyzer);

XRemoteApp* xremote_analyzer_alloc(XRemoteAppContext* app_ctx);
