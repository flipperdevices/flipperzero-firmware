#pragma once

#include <stdbool.h>
#include <stdint.h>

/*
 * BLE stack init and cleanup
 */

#ifdef __cplusplus
extern "C" {
#endif

bool ble_stack_init();

void ble_stack_get_key_storage_buff(uint8_t** addr, uint16_t* size);

void ble_stack_deinit();

#ifdef __cplusplus
}
#endif
