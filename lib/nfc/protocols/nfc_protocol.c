#include "nfc_protocol.h"

#include <furi/furi.h>

typedef struct {
    NfcProtocol parent_protocol;
    size_t children_num;
    const NfcProtocol* children_protocol;
} NfcProtocolTreeNode;

/**************************** Protocol tree structure ****************************
 *
 *                                                   (Start)
 *                                                      |
 *                             +------------------------+-----------+---------+------------+
 *                             |                        |           |         |            |
 *                        ISO14443-3A              ISO14443-3B    Felica  ISO15693-3    ST25TB
 *                             |                        |                     |
 *             +---------------+-------------+     ISO14443-4B              SLIX
 *             |               |             |
 *        ISO14443-4A   Mf Ultralight   Mf Classic
 *             |
 *       +-----+-----+
 *       |           |
 *  Mf Desfire   Bank Card
 *
 */

static const NfcProtocol nfc_protocol_iso14443_3a_children_protocol[] = {
    NfcProtocolIso14443_4a,
    NfcProtocolMfUltralight,
};

static const NfcProtocol nfc_protocol_iso14443_3b_children_protocol[] = {
    NfcProtocolIso14443_4b,
};

static const NfcProtocol nfc_protocol_iso14443_4a_children_protocol[] = {
    NfcProtocolMfDesfire,
};

static const NfcProtocol nfc_protocol_iso15693_3_children_protocol[] = {
    NfcProtocolSlix,
};

static const NfcProtocolTreeNode nfc_protocol_nodes[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = COUNT_OF(nfc_protocol_iso14443_3a_children_protocol),
            .children_protocol = nfc_protocol_iso14443_3a_children_protocol,
        },
    [NfcProtocolIso14443_3b] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = COUNT_OF(nfc_protocol_iso14443_3b_children_protocol),
            .children_protocol = nfc_protocol_iso14443_3b_children_protocol,
        },
    [NfcProtocolIso14443_4a] =
        {
            .parent_protocol = NfcProtocolIso14443_3a,
            .children_num = COUNT_OF(nfc_protocol_iso14443_4a_children_protocol),
            .children_protocol = nfc_protocol_iso14443_4a_children_protocol,
        },
    [NfcProtocolIso14443_4b] =
        {
            .parent_protocol = NfcProtocolIso14443_3b,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolIso15693_3] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = COUNT_OF(nfc_protocol_iso15693_3_children_protocol),
            .children_protocol = nfc_protocol_iso15693_3_children_protocol,
        },
    [NfcProtocolFelica] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = 0,
            .children_protocol = NULL,
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
    [NfcProtocolSlix] =
        {
            .parent_protocol = NfcProtocolIso15693_3,
            .children_num = 0,
            .children_protocol = NULL,
        },
    [NfcProtocolSt25tb] =
        {
            .parent_protocol = NfcProtocolInvalid,
            .children_num = 0,
            .children_protocol = NULL,
        },
};

NfcProtocol nfc_protocol_get_parent(NfcProtocol protocol) {
    furi_assert(protocol < NfcProtocolNum);

    return nfc_protocol_nodes[protocol].parent_protocol;
}

bool nfc_protocol_has_parent(NfcProtocol protocol, NfcProtocol parent_protocol) {
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(parent_protocol < NfcProtocolNum);

    bool parent_found = false;
    const NfcProtocolTreeNode* iter = &nfc_protocol_nodes[protocol];

    while(iter->parent_protocol != NfcProtocolInvalid) {
        if(iter->parent_protocol == parent_protocol) {
            parent_found = true;
            break;
        }
        iter = &nfc_protocol_nodes[iter->parent_protocol];
    }

    return parent_found;
}
