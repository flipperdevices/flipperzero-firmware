#include "nfc_protocol_defs.h"

#include <nfc/protocols/nfca/nfca.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>

const NfcProtocolBase* nfc_protocols[NfcProtocolTypeMax] = {
    [NfcProtocolTypeIso14443_3a] = &nfc_protocol_iso14443_3a,
    [NfcProtocolTypeIso14443_4a] = &nfc_protocol_iso14443_4a,
    [NfcProtocolTypeMfUltralight] = &nfc_protocol_mf_ultralight,
    [NfcProtocolTypeMfClassic] = &nfc_protocol_mf_classic,
    [NfcProtocolTypeMfDesfire] = &nfc_protocol_mf_desfire,
    /* Add new protocols here */
};
