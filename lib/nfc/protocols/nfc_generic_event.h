#pragma once

#include "nfc_protocol.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void NfcGenericInstance;

typedef void NfcGenericEventData;

typedef struct {
    NfcProtocol protocol;
    NfcGenericInstance* instance;
    NfcGenericEventData* event_data;
} NfcGenericEvent;

typedef NfcCommand (*NfcGenericCallback)(NfcGenericEvent event, void* context);

#ifdef __cplusplus
}
#endif
