#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include <infrared.h>
#include <infrared_worker.h>
#include <infrared_transmit.h>
#include <flipper_format/flipper_format.h>
#include "xremote_icons.h"

typedef enum {
    XRemoteRemoteItemTypeInfrared,
    XRemoteRemoteItemTypeSubGhz,
    XRemoteRemoteItemTypePause,
} XRemoteRemoteItemType;

struct InfraredSignal {
    bool is_raw;
    union {
        InfraredMessage message;
        InfraredRawSignal raw;
    } payload;
};

struct CrossRemoteItem {
    FuriString* name;
    InfraredSignal* ir_signal;
    int16_t type;
    int32_t time;
};