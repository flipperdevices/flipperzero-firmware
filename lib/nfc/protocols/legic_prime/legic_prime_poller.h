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

/** Outcome of a read or a write, reported with Success and Failure. */
typedef struct {
    LegicPrimeError error; /**< LegicPrimeErrorNone on Success. */
    /** Read: bytes read (two agreeing reads each).  Write: bytes the tag
     *  acknowledged, the last ones of the range, as they are written last to
     *  first. */
    uint16_t bytes_done;
    /** Bytes requested: the card size for a read, write_size for a write.
     *  0 if the card was not detected. */
    uint16_t bytes_total;
    /** Write failure: the WRITE of the byte just before the written ones
     *  went out and its ACK never came, so that byte may hold the new value
     *  or the old one. */
    bool write_unacked;
} LegicPrimePollerEventDataResult;

typedef union {
    LegicPrimePollerEventDataModeRequest mode_request;
    LegicPrimePollerEventDataResult result;
} LegicPrimePollerEventData;

typedef struct {
    LegicPrimePollerEventType type;
    LegicPrimePollerEventData* data;
} LegicPrimePollerEvent;

#ifdef __cplusplus
}
#endif
