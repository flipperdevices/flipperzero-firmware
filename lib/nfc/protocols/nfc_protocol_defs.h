#pragma once

#include "nfc_protocol_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcProtocolTypeIso14443_3a,
    NfcProtocolTypeIso14443_4a,
    NfcProtocolTypeMfUltralight,
    NfcProtocolTypeMfClassic,
    NfcProtocolTypeMfDesfire,
    /* Add new protocols here */

    NfcProtocolTypeMax,
} NfcProtocolType;

extern const NfcProtocolBase* nfc_protocols[];

#ifdef __cplusplus
}
#endif
