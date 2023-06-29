#include "nfc_listener_defs.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_listener_defs.h>
// #include <nfc/protocols/iso14443_4a/iso14443_4a_listener_defs.h>
// #include <nfc/protocols/mf_ultralight/mf_ultralight_listener_defs.h>
// #include <nfc/protocols/mf_classic/mf_classic_listener_defs.h>

const NfcListenerBase* nfc_listeners_api[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_listener_iso14443_3a,
    [NfcProtocolIso14443_4a] = NULL, //&nfc_listener_iso14443_4a,
    [NfcProtocolMfUltralight] = NULL, //&mf_ultralight_listener,
    [NfcProtocolMfClassic] = NULL, //&mf_classic_listener,
    [NfcProtocolMfDesfire] = NULL,
};
