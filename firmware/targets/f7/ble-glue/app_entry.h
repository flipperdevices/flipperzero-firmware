#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*BleGlueKeyStorageChangedCallback)(uint8_t* change_addr_start, uint16_t size, void* context);

typedef enum {
    BleGlueStatusUninitialized,
    BleGlueStatusStartup,
    BleGlueStatusBleStackMissing,
    BleGlueStatusStarted
} BleGlueStatus;

void APPE_Init();

BleGlueStatus APPE_Status();
void ble_glue_set_key_storage_changed_callback(BleGlueKeyStorageChangedCallback callback, void* context);

#ifdef __cplusplus
}
#endif
