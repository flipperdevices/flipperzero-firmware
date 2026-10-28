#pragma once

#include "felica_seac.h"

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FelicaSeacPoller FelicaSeacPoller;

typedef enum {
    FelicaSeacPollerEventTypeError,
    FelicaSeacPollerEventTypeReady,
} FelicaSeacPollerEventType;

typedef union {
    FelicaSeacError error;
} FelicaSeacPollerEventData;

typedef struct {
    FelicaSeacPollerEventType type;
    FelicaSeacPollerEventData* data;
} FelicaSeacPollerEvent;

#ifdef __cplusplus
}
#endif
