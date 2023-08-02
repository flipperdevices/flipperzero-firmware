#include "nfc_device_defs.h"

#include "nfc_protocol.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3b/iso14443_3b.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a.h>
#include <nfc/protocols/iso15693_3/iso15693_3.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>

const NfcDeviceBase* nfc_devices[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_device_iso14443_3a,
    [NfcProtocolIso14443_3b] = &nfc_device_iso14443_3b,
    [NfcProtocolIso14443_4a] = &nfc_device_iso14443_4a,
    [NfcProtocolIso15693_3] = &nfc_device_iso15693_3,
    [NfcProtocolMfUltralight] = &nfc_device_mf_ultralight,
    [NfcProtocolMfClassic] = &nfc_device_mf_classic,
    [NfcProtocolMfDesfire] = &nfc_device_mf_desfire,
    /* Add new protocols here */
};
