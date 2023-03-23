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
#include "../../xremote_i.h"

#include <infrared.h>
//#include <flipper_format/flipper_format.h>

typedef struct CrossRemote CrossRemote;

CrossRemote* cross_remote_alloc();

void cross_remote_free(CrossRemote* cross_remote);
bool cross_remote_load(CrossRemote* cross_remote, FuriString* path);
const char* cross_remote_get_name(CrossRemote* remote);
void cross_remote_set_transmitting(CrossRemote* remote, int status);
int cross_remote_get_transmitting(CrossRemote* remote);
bool cross_remote_add_ir_item(CrossRemote* remote, const char* name, InfraredSignal* signal);
size_t cross_remtoe_get_item_count(CrossRemote* remote);
CrossRemoteItem* cross_remote_get_item(CrossRemote* remote, size_t index);

void cross_remote_set_name(CrossRemote* remote, const char* name);
void cross_remote_set_path(CrossRemote* remote, const char* path);
bool cross_remote_save_new(CrossRemote* remote, const char* name);
bool cross_remote_store(CrossRemote* remote);