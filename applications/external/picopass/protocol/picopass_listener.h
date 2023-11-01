#pragma once

#include <nfc/nfc.h>
#include "picopass_protocol.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PicopassListenerModeEmulation,
    PicopassListenerModeLoclass,
} PicopassListenerMode;

typedef enum {
    PicopassListenerEventTypeLoclassGotStandardKey,
    PicopassListenerEventTypeLoclassGotMac,
} PicopassListenerEventType;

typedef struct {
    PicopassListenerEventType type;
} PicopassListenerEvent;

typedef NfcCommand (*PicopassListenerCallback)(PicopassListenerEvent event, void* context);

typedef struct PicopassListener PicopassListener;

PicopassListener* picopass_listener_alloc(Nfc* nfc, const PicopassDeviceData* data);

void picopass_listener_free(PicopassListener* instance);

bool picopass_listener_set_mode(PicopassListener* instance, PicopassListenerMode mode);

void picopass_listener_start(
    PicopassListener* instance,
    PicopassListenerCallback callback,
    void* context);

void picopass_listener_stop(PicopassListener* instance);

const PicopassDeviceData* picopass_listener_get_data(PicopassListener* instance);

#ifdef __cplusplus
}
#endif
