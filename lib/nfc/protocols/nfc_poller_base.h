#pragma once

#include "nfc_poller_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef NfcPollerInstance* (*NfcPollerAlloc)(NfcPollerInstance* base_poller);
typedef void (*NfcPollerFree)(NfcPollerInstance* instance);

typedef void (
    *NfcPollerSetCallback)(NfcPollerInstance* poller, NfcPollerCallback callback, void* context);
typedef NfcCommand (*NfcPollerRun)(NfcPollerEvent event, void* context);
typedef bool (*NfcPollerDetect)(NfcPollerEvent event, void* context);
typedef const NfcDeviceData* (*NfcPollerGetData)(const NfcPollerInstance* instance);

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
