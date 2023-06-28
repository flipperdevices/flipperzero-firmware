#pragma once

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

#ifdef __cplusplus
}
#endif
