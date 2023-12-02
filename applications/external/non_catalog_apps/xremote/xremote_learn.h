/*!
 *  @file flipper-xremote/xremote_learn.h
    @license This project is released under the GNU GPLv3 License
 *  @copyright (c) 2023 Sandro Kalatozishvili (s.kalatoz@gmail.com)
 *
 * @brief Functionality to read and store input from new remote.
 */

#pragma once

#include "xremote_app.h"
#include "xremote_signal.h"

typedef struct XRemoteLearnContext XRemoteLearnContext;

void xremote_learn_send_event(XRemoteLearnContext* learn_ctx, XRemoteEvent event);
const char* xremote_learn_get_curr_button_name(XRemoteLearnContext* learn_ctx);
int xremote_learn_get_curr_button_index(XRemoteLearnContext* learn_ctx);
bool xremote_learn_has_buttons(XRemoteLearnContext* learn_ctx);

XRemoteSignalReceiver* xremote_learn_get_ir_receiver(XRemoteLearnContext* learn_ctx);
XRemoteAppContext* xremote_learn_get_app_context(XRemoteLearnContext* learn_ctx);
InfraredRemote* xremote_learn_get_ir_remote(XRemoteLearnContext* learn_ctx);
InfraredSignal* xremote_learn_get_ir_signal(XRemoteLearnContext* learn_ctx);

XRemoteApp* xremote_learn_alloc(XRemoteAppContext* app_ctx);
