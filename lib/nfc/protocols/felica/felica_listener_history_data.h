#pragma once

#include "felica.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Felica_ListenerStateIdle,
    Felica_ListenerStateActivated,
} FelicaListenerState;

typedef struct {
    NfcEventType event;
    FelicaListenerState state;
    FelicaError error;
    NfcCommand command;
} FelicaListenerHistoryData;

#ifdef __cplusplus
}
#endif
