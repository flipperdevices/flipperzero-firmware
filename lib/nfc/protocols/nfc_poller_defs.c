#include "nfc_poller_defs.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller_defs.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller_defs.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight_poller_defs.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_defs.h>
#include <nfc/protocols/mf_desfire/mf_desfire_poller_defs.h>

#include <furi/core/core_defines.h>

const NfcPollerBase* nfc_pollers_api[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &nfc_poller_iso14443_3a,
    [NfcProtocolIso14443_4a] = &nfc_poller_iso14443_4a,
    [NfcProtocolMfUltralight] = &mf_ultralight_poller,
    [NfcProtocolMfClassic] = &mf_classic_poller,
    [NfcProtocolMfDesfire] = &mf_desfire_poller,
};

/**************************** Poller tree structure ****************************
 *                               _________ start ___________________________
 *                              /                         |        |        \
 *               _________ iso14443-3a _______          nfc-b    nfc-f     nfc-v
 *              /               |             \
 *        iso14443-4a     mf ultralight    mf classic
 *        /         \
 *   mf desfire  bank card
 */

static const NfcProtocol nfc_poller_iso14443_3a_children_protocol[] = {
    NfcProtocolIso14443_4a,
    NfcProtocolMfUltralight,
};

static const NfcProtocol nfc_poller_iso14443_4a_children_protocol[] = {
    NfcProtocolMfDesfire,
};

const NfcPollerTreeNode nfc_poller_nodes[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = COUNT_OF(nfc_poller_iso14443_3a_children_protocol),
            .children_protocol = nfc_poller_iso14443_3a_children_protocol,
        },
    [NfcProtocolIso14443_4a] =
        {
            .parent_protocol = NfcProtocolIso14443_3a,
            .children_num = COUNT_OF(nfc_poller_iso14443_4a_children_protocol),
            .children_protocol = nfc_poller_iso14443_4a_children_protocol,
        },
    [NfcProtocolMfUltralight] =
        {
            .parent_protocol = NfcProtocolIso14443_3a,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolMfClassic] =
        {
            .parent_protocol = NfcProtocolIso14443_3a,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolMfDesfire] =
        {
            .parent_protocol = NfcProtocolIso14443_4a,
            .children_num = 0,
            .children_protocol = NULL,
        },
};
