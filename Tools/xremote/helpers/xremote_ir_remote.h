#pragma once

#include <stdbool.h>
#include "xremote_ir_remote_button.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* xremote_ir_remote_alloc();
void xremote_ir_remote_free(InfraredRemote* remote);