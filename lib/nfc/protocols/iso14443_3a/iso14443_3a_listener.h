#pragma once

#include "iso14443_3a.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Iso14443_3aListener Iso14443_3aListener;

typedef enum {
    Iso14443_3aListenerEventConfigRequest,
    Iso14443_3aListenerEventTypeAbort,
    Iso14443_3aListenerEventTypeFieldOn,
    Iso14443_3aListenerEventTypeFieldOff,
    Iso14443_3aListenerEventTypeHalted,
    Iso14443_3aListenerEventTypeReceivedStandartFrame,
    Iso14443_3aListenerEventTypeReceivedData,
    Iso14443_3aListenerEventTypeReset,
} Iso14443_3aListenerEventType;

typedef struct {
    BitBuffer* buffer;
} Iso14443_3aListenerEventData;

typedef struct {
    Iso14443_3aListenerEventType type;
    Iso14443_3aListenerEventData data;
} Iso14443_3aListenerEvent;

typedef enum {
    Iso14443_3aListenerCommandContinue = NfcCommandContinue,
    Iso14443_3aListenerCommandReset = NfcCommandReset,
} Iso14443_3aListenerCommand;

typedef Iso14443_3aListenerCommand (
    *Iso14443_3aListenerEventCallback)(Iso14443_3aListenerEvent event, void* context);

Iso14443_3aListener* iso14443_3a_listener_alloc(Nfc* nfc);

void iso14443_3a_listener_free(Iso14443_3aListener* instance);

Iso14443_3aError iso14443_3a_listener_start(
    Iso14443_3aListener* instance,
    const Iso14443_3aData* data,
    Iso14443_3aListenerEventCallback callback,
    void* context);

Iso14443_3aError
    iso14443_3a_listener_get_data(Iso14443_3aListener* instance, Iso14443_3aData* data);

Iso14443_3aError iso14443_3a_listener_stop(Iso14443_3aListener* instance);

// Called from NfcWorker thread

Iso14443_3aError iso14443_3a_listener_sleep(Iso14443_3aListener* instance);

Iso14443_3aError
    iso14443_3a_listener_tx(Iso14443_3aListener* instance, const BitBuffer* tx_buffer);

Iso14443_3aError iso14443_3a_listener_send_standart_frame(
    Iso14443_3aListener* instance,
    const BitBuffer* tx_buffer);

#ifdef __cplusplus
}
#endif
