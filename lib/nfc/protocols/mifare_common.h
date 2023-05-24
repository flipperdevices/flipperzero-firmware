#pragma once

#include <stdint.h>
#include "furi_hal_nfc.h"

typedef enum {
    MifareTypeUnknown,
    MifareTypeUltralight,
    MifareTypeClassic,
    MifareTypeDesfire,
} MifareType;

MifareType mifare_common_get_type(FuriHalNfcADevData* data);
