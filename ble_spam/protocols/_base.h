#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assets_icons.h>
#include "ble_spam_icons.h"
#include <furi_hal_random.h>
#include <core/core_defines.h>

typedef union BleSpamMsg BleSpamMsg;

typedef struct {
    const Icon* icon;
    const char* (*get_name)(const BleSpamMsg* _msg);
    void (*make_packet)(uint8_t* out_size, uint8_t** out_packet, const BleSpamMsg* _msg);
} BleSpamProtocol;
