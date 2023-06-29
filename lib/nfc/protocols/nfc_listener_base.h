#pragma once

#include "nfc_generic_event.h"
#include "nfc_device_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef NfcGenericInstance* (
    *NfcListenerAlloc)(NfcGenericInstance* base_listener, NfcDeviceData* data);
typedef void (*NfcListenerFree)(NfcGenericInstance* instance);

typedef void (*NfcListenerSetCallback)(
    NfcGenericInstance* listener,
    NfcGenericCallback callback,
    void* context);
typedef NfcCommand (*NfcListenerRun)(NfcGenericEvent event, void* context);

typedef const NfcDeviceData* (*NfcListenerGetData)(const NfcGenericInstance* instance);

typedef struct {
    NfcListenerAlloc alloc;
    NfcListenerFree free;
    NfcListenerSetCallback set_callback;
    NfcListenerRun run;
    NfcListenerGetData get_data;
} NfcListenerBase;

#ifdef __cplusplus
}
#endif
