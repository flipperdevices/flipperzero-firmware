#pragma once

#include "nfc_poller_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NfcProtocol parent_protocol;
    size_t children_num;
    const NfcProtocol* children_protocol;
} NfcPollerTreeNode;

extern const NfcPollerBase* nfc_pollers_api[NfcProtocolNum];

extern const NfcPollerTreeNode nfc_poller_nodes[NfcProtocolNum];

#ifdef __cplusplus
}
#endif
