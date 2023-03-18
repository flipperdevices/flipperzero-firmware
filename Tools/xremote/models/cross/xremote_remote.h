#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include "xremote_remote_item.h"

//#include <infrared.h>
//#include <flipper_format/flipper_format.h>

typedef struct CrossRemote CrossRemote;

CrossRemote* cross_remote_alloc();
void cross_remote_free(CrossRemote* cross_remote);
bool cross_remote_load(CrossRemote* cross_remote, FuriString* path);
bool cross_remote_add_item(CrossRemote* remote, const char* name);
size_t cross_remtoe_get_item_count(CrossRemote* remote);
CrossRemoteItem* cross_remote_get_item(CrossRemote* remote, size_t index);