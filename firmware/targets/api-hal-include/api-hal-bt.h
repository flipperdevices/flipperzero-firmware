#pragma once

#include <m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize */
void api_hal_bt_init();

/* Get BT/BLE system component state */
void api_hal_bt_dump_state(string_t buffer);

#ifdef __cplusplus
}
#endif
