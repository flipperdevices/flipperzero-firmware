#pragma once

#include "nfc_device_defs.h"
#include "nfc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void NfcGenericInstance;

typedef void NfcGenericEventData;

typedef struct {
    NfcProtocol protocol;
    NfcGenericInstance* poller;
    NfcGenericEventData* data;
} NfcGenericEvent;

typedef NfcCommand (*NfcGenericCallback)(NfcGenericEvent event, void* context);

#ifdef __cplusplus
}
#endif
