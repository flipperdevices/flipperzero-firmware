#pragma once

#include "nfca.h"
#include <lib/nfc/nfc.h>

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NfcaPoller NfcaPoller;

typedef enum {
    NfcaPollerEventTypeError,
    NfcaPollerEventTypeReady,
} NfcaPollerEventType;

typedef struct {
    NfcaError error;
} NfcaPollerEventData;

typedef struct {
    NfcaPollerEventType type;
    NfcaPollerEventData* data;
} NfcaPollerEvent;

#ifdef __cplusplus
}
#endif
