#include "nfc_cli_protocol_support_defs.h"

#include <stddef.h>

#include "iso14443_3a/iso14443_3a.h"

const NfcCliProtocolSupportBase* nfc_cli_protocol_support[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_cli_protocol_support_base_iso14443_3a,
    [NfcProtocolIso14443_3b] = NULL,
    [NfcProtocolIso14443_4a] = NULL,
    [NfcProtocolIso14443_4b] = NULL,
    [NfcProtocolIso15693_3] = NULL,
    [NfcProtocolFelica] = NULL,
    [NfcProtocolMfUltralight] = NULL,
    [NfcProtocolMfClassic] = NULL,
    [NfcProtocolMfDesfire] = NULL,
    [NfcProtocolSlix] = NULL,
    [NfcProtocolSt25tb] = NULL,
};
