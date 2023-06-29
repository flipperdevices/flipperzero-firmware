#pragma once

#include "nfc_generic_event.h"
#include "nfc_device_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef NfcGenericInstance* (*NfcListenerAlloc)(NfcGenericInstance* base_listener);
typedef void (*NfcListenerFree)(NfcGenericInstance* instance);

typedef const NfcDeviceData* (*NfcListenerGetBaseData)(const NfcGenericInstance* instance);
typedef void (*NfcListenerSetData)(NfcGenericInstance* instance, const NfcDeviceData* data);

typedef void (*NfcListenerSetCallback)(
    NfcGenericInstance* listener,
    NfcGenericCallback callback,
    void* context);
typedef NfcCommand (*NfcListenerRun)(NfcGenericEvent event, void* context);

typedef const NfcDeviceData* (*NfcListenerGetData)(const NfcGenericInstance* instance);

typedef struct {
    NfcListenerAlloc alloc;
    NfcListenerFree free;
    NfcListenerGetBaseData get_base_data;
    NfcListenerSetData set_data;
    NfcListenerSetCallback set_callback;
    NfcListenerRun run;
    NfcListenerGetData get_data;
} NfcListenerBase;

#ifdef __cplusplus
}
#endif
