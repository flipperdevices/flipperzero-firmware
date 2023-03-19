#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include "xremote_ir_remote_button.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* xremote_ir_remote_alloc();
void xremote_ir_remote_free(InfraredRemote* remote);

const char* xremote_ir_remote_get_name(InfraredRemote* remote);

bool xremote_ir_remote_load(InfraredRemote* remote, FuriString* path);
size_t xremote_ir_remote_get_button_count(InfraredRemote* remote);
InfraredRemoteButton* xremote_ir_remote_get_button(InfraredRemote* remote, size_t index);