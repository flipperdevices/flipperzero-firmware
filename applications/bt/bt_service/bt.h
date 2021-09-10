#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Bt Bt;

bool bt_update_battery_level(Bt* bt, uint8_t battery_level);

#ifdef __cplusplus
}
#endif
