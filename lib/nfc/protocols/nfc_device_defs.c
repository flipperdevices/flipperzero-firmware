#include "nfc_device_defs.h"

#include "nfc_protocol.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_device_defs.h>
#include <nfc/protocols/iso14443_3b/iso14443_3b_device_defs.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_device_defs.h>
#include <nfc/protocols/iso14443_4b/iso14443_4b_device_defs.h>
#include <nfc/protocols/iso15693_3/iso15693_3_device_defs.h>
#include <nfc/protocols/felica/felica.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_desfire/mf_desfire.h>
#include <nfc/protocols/slix/slix_device_defs.h>
#include <nfc/protocols/st25tb/st25tb.h>

const NfcDeviceBase* nfc_devices[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_device_iso14443_3a,
    [NfcProtocolIso14443_3b] = &nfc_device_iso14443_3b,
    [NfcProtocolIso14443_4a] = &nfc_device_iso14443_4a,
    [NfcProtocolIso14443_4b] = &nfc_device_iso14443_4b,
    [NfcProtocolIso15693_3] = &nfc_device_iso15693_3,
    [NfcProtocolFelica] = &nfc_device_felica,
    [NfcProtocolMfUltralight] = &nfc_device_mf_ultralight,
    [NfcProtocolMfClassic] = &nfc_device_mf_classic,
    [NfcProtocolMfDesfire] = &nfc_device_mf_desfire,
    [NfcProtocolSlix] = &nfc_device_slix,
    [NfcProtocolSt25tb] = &nfc_device_st25tb,
    /* Add new protocols here */
};
