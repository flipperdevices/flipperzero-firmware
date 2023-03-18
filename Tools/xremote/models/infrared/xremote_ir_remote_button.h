#pragma once

#include "xremote_ir_signal.h"
//#include "../xremote.h"

typedef struct InfraredRemoteButton InfraredRemoteButton;

InfraredRemoteButton* xremote_ir_remote_button_alloc();
void xremote_ir_remote_button_free(InfraredRemoteButton* button);
void xremote_ir_remote_button_set_name(InfraredRemoteButton* button, const char* name);
const char* xremote_ir_remote_button_get_name(InfraredRemoteButton* button);
InfraredSignal* xremote_ir_remote_button_get_signal(InfraredRemoteButton* button);