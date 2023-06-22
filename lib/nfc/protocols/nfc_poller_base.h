#pragma once

#include "nfc_protocol_defs.h"
#include "nfc.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void NfcPoller;

typedef void NfcPollerEventData;

typedef struct {
    NfcProtocolType protocol_type;
    NfcPoller* poller;
    NfcPollerEventData* data;
} NfcPollerEvent;

typedef NfcCommand (*NfcPollerCallback)(NfcPollerEvent event, void* context);

typedef NfcPoller* (*NfcPollerAlloc)(NfcPoller* base_poller);
typedef void (*NfcPollerFree)(NfcPoller* instance);

typedef void (*NfcPollerSetCallback)(NfcPoller* poller, NfcPollerCallback callback, void* context);
typedef NfcCommand (*NfcPollerRun)(NfcPollerEvent event, void* context);
typedef bool (*NfcPollerDetect)(NfcPollerEvent event, void* context);
typedef const NfcProtocolData* (*NfcPollerGetData)(const NfcPoller* instance);

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
