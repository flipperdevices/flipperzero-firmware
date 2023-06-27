#pragma once

#include "nfc_protocol_defs.h"
#include "nfc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void NfcPollerInstance;

typedef void NfcPollerEventData;

typedef struct {
    NfcProtocol protocol;
    NfcPollerInstance* poller;
    NfcPollerEventData* data;
} NfcPollerEvent;

typedef NfcCommand (*NfcPollerCallback)(NfcPollerEvent event, void* context);

#ifdef __cplusplus
}
#endif
