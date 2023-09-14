#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcProtocolIso14443_3a,
    NfcProtocolIso14443_3b,
    NfcProtocolIso14443_4a,
    NfcProtocolIso14443_4b,
    NfcProtocolIso15693_3,
    NfcProtocolFelica,
    NfcProtocolMfUltralight,
    NfcProtocolMfClassic,
    NfcProtocolMfDesfire,
    NfcProtocolSlix,
    /* Add new protocols here */

    NfcProtocolNum,

    NfcProtocolInvalid,
} NfcProtocol;

NfcProtocol nfc_protocol_get_parent(NfcProtocol protocol);

bool nfc_protocol_has_parent(NfcProtocol protocol, NfcProtocol parent_protocol);

#ifdef __cplusplus
}
#endif
