#pragma once

#include "legic_prime.h"
#include <lib/nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LegicPrimePoller LegicPrimePoller;

/**
 * Events of the LEGIC Prime poller.  A read or write runs from RequestMode to
 * one Success or Failure (a Failure comes without a RequestMode when no card
 * completes the handshake), and its session budget bounds that read or write,
 * not the poller: on NfcCommandContinue to Success or Failure the poller
 * starts over with a new handshake and RequestMode and a fresh budget, so it
 * runs until the callback answers NfcCommandStop or nfc_poller_stop() is
 * called.
 */
typedef enum {
    LegicPrimePollerEventTypeRequestMode, /**< Fill in data->mode_request. */
    LegicPrimePollerEventTypeSuccess, /**< Read or write done, see data->result. */
    LegicPrimePollerEventTypeFailure, /**< Read, write or handshake failed, see data->result. */
} LegicPrimePollerEventType;

typedef enum {
    LegicPrimePollerModeRead,
    LegicPrimePollerModeWrite,

    LegicPrimePollerModeNum,
} LegicPrimePollerMode;

typedef struct {
    LegicPrimePollerMode mode;
    /** Read mode: read_size bytes from read_offset on, clamped to the card,
     *  or the whole card if read_size is 0.  Of the bytes outside the range
     *  only the UID is filled in, so a range read is no dump to save.  A
     *  range that starts at or past the end fails with
     *  LegicPrimeErrorReadOnly. */
    uint16_t read_offset;
    uint16_t read_size;
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
    /** Read: bytes of the range read (two agreeing reads each).  Write:
     *  bytes the tag acknowledged, the last ones of the range, as they are
     *  written last to first. */
    uint16_t bytes_done;
    /** Bytes requested: for a read the range clamped to the card (the card
     *  size for a whole-card read), for a write write_size.  0 if the card
     *  was not detected, or if a read range starts at or past the card's
     *  end (LegicPrimeErrorReadOnly once a session confirmed the UID,
     *  another error if none did). */
    uint16_t bytes_total;
    /** Write failure: the WRITE of the byte just before the written ones
     *  went out and its ACK never came, so that byte may hold the new value
     *  or the old one.  A WRITE chained behind that missing ACK may have gone
     *  out as well, so the byte before it is unconfirmed too; read the range
     *  back to be sure. */
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
