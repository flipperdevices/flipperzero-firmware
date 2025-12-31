#pragma once

#include "felica.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FelicaPollerStateIdle,
    FelicaPollerStateActivated,
    FelicaPollerStateAuthenticateInternal,
    FelicaPollerStateAuthenticateExternal,
    FelicaPollerStateReadBlocks,
    FelicaPollerStateReadSuccess,
    FelicaPollerStateReadFailed,

    FelicaPollerStateNum
} FelicaPollerState;

typedef struct {
    NfcEventType event;
    FelicaPollerState state;
    FelicaError error;
    NfcCommand command;
} FURI_PACKED FelicaPollerHistoryData;

#ifdef __cplusplus
}
#endif
