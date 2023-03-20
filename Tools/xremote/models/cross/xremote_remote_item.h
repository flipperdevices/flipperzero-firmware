#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include <infrared.h>
#include <flipper_format/flipper_format.h>

typedef struct CrossRemoteItem CrossRemoteItem;

CrossRemoteItem* xremote_remote_item_alloc();
void xremote_remote_item_free(CrossRemoteItem* item);

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name);
const char* xremote_remote_item_get_name(CrossRemoteItem* item);

void xremote_remote_item_set_type(CrossRemoteItem* item, int type);

InfraredSignal* xremote_remote_item_get_ir_signal(CrossRemoteItem* item);
void xremote_remote_item_set_ir_signal(CrossRemoteItem* item, InfraredSignal* signal);

bool xremote_ir_signal_save(InfraredSignal* signal, FlipperFormat* ff, const char* name);



