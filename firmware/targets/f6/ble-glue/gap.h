#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    APP_BLE_IDLE,
    APP_BLE_FAST_ADV,
    APP_BLE_LP_ADV,
    APP_BLE_SCAN,
    APP_BLE_LP_CONNECTING,
    APP_BLE_CONNECTED_SERVER,
    APP_BLE_CONNECTED_CLIENT
} APP_BLE_ConnStatus_t;

bool gap_init();

APP_BLE_ConnStatus_t gap_get_connection_status();

#ifdef __cplusplus
}
#endif
