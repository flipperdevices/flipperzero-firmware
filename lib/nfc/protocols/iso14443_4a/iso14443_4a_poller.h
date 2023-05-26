#pragma once

#include <lib/nfc/protocols/nfca/nfca_poller.h>

#include "iso14443_4a.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_4aPoller Iso14443_4aPoller;

typedef enum {
    Iso14443_4aPollerEventTypeError,
    Iso14443_4aPollerEventTypeReady,
} Iso14443_4aPollerEventType;

typedef struct {
    Iso14443_4aError error;
} Iso14443_4aPollerEventData;

typedef struct {
    Iso14443_4aPollerEventType type;
    Iso14443_4aPollerEventData* data;
} Iso14443_4aPollerEvent;

typedef enum {
    Iso14443_4aPollerCommandContinue = NfcaPollerCommandContinue,
    Iso14443_4aPollerCommandReset = NfcaPollerCommandReset,
    Iso14443_4aPollerCommandStop = NfcaPollerCommandStop,
} Iso14443_4aPollerCommand;

typedef Iso14443_4aPollerCommand (
    *Iso14443_4aPollerCallback)(Iso14443_4aPollerEvent event, void* context);

Iso14443_4aPoller* iso14443_4a_poller_alloc(NfcaPoller* iso14443_3a_poller);

void iso14443_4a_poller_free(Iso14443_4aPoller* instance);

Iso14443_4aError iso14443_4a_poller_start(
    Iso14443_4aPoller* instance,
    NfcaPollerEventCallback callback,
    void* context);

Iso14443_4aError iso14443_4a_poller_read(
    Iso14443_4aPoller* instance,
    Iso14443_4aPollerCallback callback,
    void* context);

Iso14443_4aError iso14443_4a_poller_stop(Iso14443_4aPoller* instance);

Iso14443_4aError iso14443_4a_poller_get_data(Iso14443_4aPoller* instance, Iso14443_4aData* data);

#ifdef __cplusplus
}
#endif
