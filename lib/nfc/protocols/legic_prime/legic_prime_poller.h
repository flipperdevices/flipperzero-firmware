#pragma once

#include "legic_prime.h"
#include <lib/nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LegicPrimePoller LegicPrimePoller;

typedef enum {
    LegicPrimePollerEventTypeRequestMode,
    LegicPrimePollerEventTypeSuccess,
    LegicPrimePollerEventTypeFailure,
} LegicPrimePollerEventType;

typedef enum {
    LegicPrimePollerModeRead,
    LegicPrimePollerModeWrite,

    LegicPrimePollerModeNum,
} LegicPrimePollerMode;

typedef struct {
    LegicPrimePollerMode mode;
    /** Write mode: bytes to write from write_offset on (offset >= 5), sent
     *  last byte first.  The buffer must stay valid until the poller reports
     *  success or failure. */
    uint16_t write_offset;
    const uint8_t* write_data;
    uint16_t write_size;
} LegicPrimePollerEventDataModeRequest;

typedef union {
    LegicPrimePollerEventDataModeRequest mode_request;
    LegicPrimeError error;
} LegicPrimePollerEventData;

typedef struct {
    LegicPrimePollerEventType type;
    LegicPrimePollerEventData* data;
} LegicPrimePollerEvent;

/** Detect the tag: send the IV and decode the card type into instance->data. */
LegicPrimeError legic_prime_poller_setup(LegicPrimePoller* instance, uint8_t iv);

/**
 * Run one read session from a fresh field: handshake, then read every byte
 * not read yet.  Returns LegicPrimeErrorNone once all bytes are read; any
 * other error means the session ended early (reset the field and call again).
 */
LegicPrimeError legic_prime_poller_read(LegicPrimePoller* instance);

#ifdef __cplusplus
}
#endif
