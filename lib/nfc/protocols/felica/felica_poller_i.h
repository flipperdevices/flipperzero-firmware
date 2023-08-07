#pragma once

#include "felica_poller.h"

#include <nfc/helpers/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FELICA_POLLER_MAX_BUFFER_SIZE (256U)

typedef enum {
    FelicaPollerStateIdle,
    FelicaPollerStateColResInProgress,
    FelicaPollerStateColResFailed,
    FelicaPollerStateActivationInProgress,
    FelicaPollerStateActivationFailed,
    FelicaPollerStateActivated,
} FelicaPollerState;

struct FelicaPoller {
    Nfc* nfc;
    FelicaPollerState state;
    FelicaData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    NfcGenericEvent general_event;
    FelicaPollerEvent felica_event;
    FelicaPollerEventData felica_event_data;
    NfcGenericCallback callback;
    void* context;
};

const FelicaData* felica_poller_get_data(FelicaPoller* instance);

FelicaError
    felica_poller_async_activate(FelicaPoller* instance, FelicaData* data);

FelicaError felica_poller_halt(FelicaPoller* instance);

FelicaError felica_poller_send_frame(
    FelicaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

#ifdef __cplusplus
}
#endif
