#pragma once

#include <stdint.h>
#include <stdbool.h>

#define BT_SETTINGS_FILE_NAME ".bt.settings"

typedef struct {
    bool enabled;
} BtSettings;

bool bt_settings_load(BtSettings* bt_settings);

bool bt_settings_save(BtSettings* bt_settings);
