#pragma once

#include "nfc_device_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcProtocolIso14443_3a,
    NfcProtocolIso14443_4a,
    NfcProtocolMfUltralight,
    NfcProtocolMfClassic,
    NfcProtocolMfDesfire,
    /* Add new protocols here */

    NfcProtocolNum,

    NfcProtocolInvalid,
} NfcProtocol;

extern const NfcDeviceBase* nfc_devices[];

#ifdef __cplusplus
}
#endif
