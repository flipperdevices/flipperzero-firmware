#pragma once

#include <furi_hal.h>
#include "model.h"

typedef enum {
    SwitchId1 = 1,
    SwitchId2 = 1 << 1,
    SwitchId3 = 1 << 2,
    SwitchId4 = 1 << 3,
} SwitchIds;

#define IS_SWITCH_PRESSED(key, switch_id) ((key & switch_id) == switch_id)
#define IS_SWITCH_RELEASED(key, switch_id) ((key & switch_id) == 0)
#define IS_SWITCH_ALL_RELEASED(key) (key == 0)

KeyMonitor* key_monitor_alloc(void);
void key_monitor_set_callback(KeyMonitor* km, KeyMonitorEventCallback callback, void* context);
void key_monitor_free(KeyMonitor* km);
uint8_t key_monitor_get_last_status(KeyMonitor* km);