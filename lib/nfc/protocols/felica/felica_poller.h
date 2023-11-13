#pragma once

#include "felica.h"
#include <lib/nfc/nfc.h>

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FelicaPoller FelicaPoller;

typedef enum {
    FelicaPollerEventTypeError,
    FelicaPollerEventTypeReady,
} FelicaPollerEventType;

typedef struct {
    FelicaError error;
} FelicaPollerEventData;

typedef struct {
    FelicaPollerEventType type;
    FelicaPollerEventData* data;
} FelicaPollerEvent;

FelicaError felica_poller_activate(FelicaPoller* instance, FelicaData* data);

#ifdef __cplusplus
}
#endif
