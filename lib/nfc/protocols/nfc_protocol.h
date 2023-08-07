#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcProtocolFelica,
    NfcProtocolIso14443_3a,
    NfcProtocolIso14443_3b,
    NfcProtocolIso14443_4a,
    NfcProtocolIso15693_3,
    NfcProtocolMfUltralight,
    NfcProtocolMfClassic,
    NfcProtocolMfDesfire,
    /* Add new protocols here */

    NfcProtocolNum,

    NfcProtocolInvalid,
} NfcProtocol;

NfcProtocol nfc_protocol_get_parent(NfcProtocol protocol);

bool nfc_protocol_has_parent(NfcProtocol protocol, NfcProtocol parent_protocol);

#ifdef __cplusplus
}
#endif
