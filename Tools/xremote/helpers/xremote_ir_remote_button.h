#pragma once

#include "xremote_ir_signal.h"

typedef struct InfraredRemoteButton InfraredRemoteButton;

InfraredRemoteButton* xremote_ir_remote_button_alloc();
void xremote_ir_remote_button_free(InfraredRemoteButton* buton);
