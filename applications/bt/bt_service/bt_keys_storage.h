#pragma once

#include "bt_i.h"

#define BT_KEYS_STORAGE_FILE_NAME ".bt.keys"

bool bt_keys_storage_load(Bt* bt);

bool bt_keys_storage_save(Bt* bt);

bool bt_keys_storage_delete(Bt* bt);
