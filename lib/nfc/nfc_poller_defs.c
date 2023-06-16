#include "nfc_poller_defs.h"

#include <nfc/protocols/nfca/nfca_poller_i.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight_poller_i.h>

#include <furi/core/core_defines.h>

const NfcPollerBase* nfc_pollers_api[NfcProtocolTypeMax] = {
    [NfcProtocolTypeIso14443_3a] = &nfc_poller_iso14443_3a,
    [NfcProtocolTypeIso14443_4a] = &nfc_poller_iso14443_4a,
    [NfcProtocolTypeMfUltralight] = &mf_ultralight_poller,
};

const NfcProtocolType nfc_poller_base_protocol_types[] = {NfcProtocolTypeIso14443_3a};

const NfcPollerBaseProtocol nfc_poller_base_protocols = {
    .base_protocol_num = COUNT_OF(nfc_poller_base_protocol_types),
    .base_protocols = nfc_poller_base_protocol_types,
};

//              nfc-a                               nfc-b            nfc-f     nfc-v
//
//  iso14443-4a      mf ultraligh    mf classic
//
//  mf desfire

static const NfcProtocolType nfc_poller_iso14443_3a_children_protocol[] = {
    NfcProtocolTypeIso14443_4a,
    NfcProtocolTypeMfUltralight,
};

static const NfcProtocolType nfc_poller_iso14443_4a_children_protocol[] = {
    NfcProtocolTypeMfDesfire,
};

const NfcPollerTreeNode nfc_poller_nodes[NfcProtocolTypeMax] = {
    [NfcProtocolTypeIso14443_3a] =
        {
            .parent_protocol = NfcProtocolTypeInvalid,
            .children_num = COUNT_OF(nfc_poller_iso14443_3a_children_protocol),
            .children_protocol = nfc_poller_iso14443_3a_children_protocol,
        },
    [NfcProtocolTypeIso14443_4a] =
        {
            .parent_protocol = NfcProtocolTypeIso14443_3a,
            .children_num = COUNT_OF(nfc_poller_iso14443_4a_children_protocol),
            .children_protocol = nfc_poller_iso14443_4a_children_protocol,
        },
    [NfcProtocolTypeMfUltralight] =
        {
            .parent_protocol = NfcProtocolTypeIso14443_3a,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolTypeMfClassic] =
        {
            .parent_protocol = NfcProtocolTypeIso14443_3a,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolTypeMfDesfire] =
        {
            .parent_protocol = NfcProtocolTypeIso14443_4a,
            .children_num = 0,
            .children_protocol = NULL,
        },
};
