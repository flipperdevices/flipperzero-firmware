#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>

typedef struct CrossRemoteItem CrossRemoteItem;

CrossRemoteItem* xremote_remote_item_alloc();
void xremote_remote_item_free(CrossRemoteItem* item);

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name);
const char* xremote_remote_item_get_name(CrossRemoteItem* item);



