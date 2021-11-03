#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BleGlueStatusUninitialized,
    BleGlueStatusStartup,
    BleGlueStatusBleStackMissing,
    BleGlueStatusStarted
} BleGlueStatus;

void ble_glue_init();

BleGlueStatus ble_glue_get_status();

#ifdef __cplusplus
} /* extern "C" */
#endif
