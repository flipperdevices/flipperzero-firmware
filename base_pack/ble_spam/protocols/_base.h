#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ble_spam_icons.h"
#include <furi_hal_random.h>
#include <core/core_defines.h>

typedef union BleSpamProtocolCfg BleSpamProtocolCfg;

typedef struct {
    const Icon* icon;
    const char* (*get_name)(const BleSpamProtocolCfg* _cfg);
    void (*make_packet)(uint8_t* _size, uint8_t** _packet, const BleSpamProtocolCfg* _cfg);
} BleSpamProtocol;
