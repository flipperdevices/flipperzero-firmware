#pragma once

#include <stddef.h>
#include "nfc_poller_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    NfcProtocolType parent_protocol;
    size_t children_num;
    const NfcProtocolType* children_protocol;
} NfcPollerTreeNode;

typedef struct {
    size_t base_protocol_num;
    const NfcProtocolType* base_protocols;
} NfcPollerBaseProtocol;

extern const NfcPollerBase* nfc_pollers_api[NfcProtocolTypeMax];

extern const NfcPollerTreeNode nfc_poller_nodes[NfcProtocolTypeMax];

extern const NfcPollerBaseProtocol nfc_poller_base_protocols;

#ifdef __cplusplus
}
#endif
