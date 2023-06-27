#include "nfc_protocol_defs.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>

const NfcProtocolBase* nfc_devices[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_protocol_iso14443_3a,
    [NfcProtocolIso14443_4a] = &nfc_protocol_iso14443_4a,
    [NfcProtocolMfUltralight] = &nfc_protocol_mf_ultralight,
    [NfcProtocolMfClassic] = &nfc_protocol_mf_classic,
    [NfcProtocolMfDesfire] = &nfc_protocol_mf_desfire,
    /* Add new protocols here */
};
