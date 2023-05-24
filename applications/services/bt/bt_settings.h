#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BT_SETTINGS_OLD_PATH INT_PATH(".bt.settings")
#define BT_SETTINGS_PATH CFG_PATH("bt.settings")

typedef struct {
    bool enabled;
} BtSettings;

bool bt_settings_load(BtSettings* bt_settings);

bool bt_settings_save(BtSettings* bt_settings);

#ifdef __cplusplus
}
#endif