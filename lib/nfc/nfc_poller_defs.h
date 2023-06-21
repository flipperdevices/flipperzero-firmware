#pragma once

#include "nfc_poller_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NfcProtocolType parent_protocol;
    size_t children_num;
    const NfcProtocolType* children_protocol;
} NfcPollerTreeNode;

extern const NfcPollerBase* nfc_pollers_api[NfcProtocolTypeMax];

extern const NfcPollerTreeNode nfc_poller_nodes[NfcProtocolTypeMax];

#ifdef __cplusplus
}
#endif
