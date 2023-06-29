#pragma once

#include "nfc_generic_event.h"
#include "nfc_device_base.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef NfcGenericInstance* (*NfcPollerAlloc)(NfcGenericInstance* base_poller);
typedef void (*NfcPollerFree)(NfcGenericInstance* instance);

typedef void (
    *NfcPollerSetCallback)(NfcGenericInstance* poller, NfcGenericCallback callback, void* context);
typedef NfcCommand (*NfcPollerRun)(NfcGenericEvent event, void* context);
typedef bool (*NfcPollerDetect)(NfcGenericEvent event, void* context);
typedef const NfcDeviceData* (*NfcPollerGetData)(const NfcGenericInstance* instance);

typedef struct {
    NfcPollerAlloc alloc;
    NfcPollerFree free;
    NfcPollerSetCallback set_callback;
    NfcPollerRun run;
    NfcPollerDetect detect;
    NfcPollerGetData get_data;
} NfcPollerBase;

#ifdef __cplusplus
}
#endif
