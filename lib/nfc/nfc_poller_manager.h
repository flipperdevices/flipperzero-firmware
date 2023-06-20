#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "nfc_poller_base.h"

typedef struct NfcPollerManager NfcPollerManager;

typedef enum {
    NfcPollerManagerEventTypeDetected,
} NfcPollerManagerEventType;

typedef struct {
    size_t protocol_num;
    NfcProtocolType* protocols;
} NfcPollerManagerEventData;

typedef struct {
    NfcPollerManagerEventType type;
    NfcPollerManagerEventData data;
} NfcPollerManagerEvent;

NfcPollerManager* nfc_poller_manager_alloc(Nfc* nfc);

void nfc_poller_manager_free(NfcPollerManager* instance);

void nfc_poller_manager_start(
    NfcPollerManager* instance,
    NfcProtocolType protocol,
    NfcPollerCallback callback,
    void* context);

void nfc_poller_manager_stop(NfcPollerManager* instance);

#ifdef __cplusplus
}
#endif
