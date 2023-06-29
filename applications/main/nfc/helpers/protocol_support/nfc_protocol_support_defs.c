#include "nfc_protocol_support_defs.h"

#include "iso14443_3a/iso14443_3a.h"
#include "iso14443_4a/iso14443_4a.h"
#include "mf_ultralight/mf_ultralight.h"
#include "mf_classic/mf_classic.h"
#include "mf_desfire/mf_desfire.h"

const NfcProtocolSupportBase* nfc_protocol_support[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_protocol_support_iso14443_3a,
    [NfcProtocolIso14443_4a] = &nfc_protocol_support_iso14443_4a,
    [NfcProtocolMfUltralight] = &nfc_protocol_support_mf_ultralight,
    [NfcProtocolMfClassic] = &nfc_protocol_support_mf_classic,
    [NfcProtocolMfDesfire] = &nfc_protocol_support_mf_desfire,
    /* Add new protocols here */
};
