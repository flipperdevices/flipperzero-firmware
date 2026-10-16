#pragma once

#include "legic_prime_poller.h"
#include "legic_prime_i.h"

#include <nfc/nfc_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LEGIC_PRIME_POLLER_MAX_BUFFER_SIZE (8U)

// FWT (frame wait time) is the window the chip listens for tag response
// after end of reader TX. LEGIC's TAG_FRAME_WAIT is 330 µs (the moment the
// response begins). Bump to ~2.5 ms so the entire 6-bit tag response (≈600µs
// at TAG_BIT_PERIOD=100µs) lands inside the window.
#define LEGIC_PRIME_FWT_FC       (33900U) // ≈ 2500 µs
#define LEGIC_PRIME_GUARD_US     (5000U)
// The HAL times LEGIC frames itself (furi_hal_nfc_legic_poller_set_next), so
// the generic inter-frame blocking timers are kept short.
#define LEGIC_PRIME_FDT_POLL_FC  (500U)
#define LEGIC_PRIME_POLL_POLL_US (50U)

/*
 * Keystream between frames.
 *
 * A LEGIC tag advances its keystream every 99.1 µs while idle (Proxmark
 * legicrfsim.c rx_frame).  If our next frame's first pause comes at t:
 *  - after a tag reply ending at E, the tag has advanced ceil((t - E) / 99.1 µs);
 *  - after our frame without a reply, ending at A, ceil((t - A) / 99.1 µs) - 1.
 * The HAL sends queued frames 260 µs after the tag's reply and chains the
 * first READ 222 µs after the ACK (furi_hal_nfc_legic_poller_set_next), so
 * the steps are fixed: 3 after a reply and 2 after the ACK, as Proxmark's
 * reader with its 220 µs gaps.
 */
#define LEGIC_PRIME_STEPS_AFTER_REPLY      (3U)
#define LEGIC_PRIME_STEPS_AFTER_READER_ACK (2U)
// Between chained WRITEs the tag steps k + 6 times, k being the bit window
// of the ACK (see the write path in legic_prime_poller_i.c).
#define LEGIC_PRIME_WRITE_STEPS_AFTER_ACK  (6U)

// One reader frame and the keystream for its reply, built ahead of time.
typedef struct {
    uint32_t tx; // on-air bits, LSB first
    uint8_t tx_bits;
    uint32_t ks; // keystream of the reply
    uint8_t rx_bits;
    uint16_t addr;
} LegicPrimeExchange;

typedef enum {
    LegicPrimePollerStateSetup,
    LegicPrimePollerStateRequestMode,
    LegicPrimePollerStateRead,
    LegicPrimePollerStateWrite,
    LegicPrimePollerStateSuccess,
    LegicPrimePollerStateFailure,

    LegicPrimePollerStateNum,
} LegicPrimePollerState;

// Read progress of one byte: LEGIC_PRIME_POLLER_* flags, and the failed
// reads in the current session above them.
#define LEGIC_PRIME_POLLER_DONE    (1U << 0) // two reads agreed
#define LEGIC_PRIME_POLLER_SEEN    (1U << 1) // read once with a valid CRC
#define LEGIC_PRIME_POLLER_ATTEMPT (1U << 2)

struct LegicPrimePoller {
    Nfc* nfc;
    LegicPrimePollerState state;
    LegicPrimeData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    uint8_t iv; // seeds every session's keystream
    // Command width in bits (6/9/11), from the detected type.
    uint8_t cmd_size_bits;

    // Read progress per byte, kept across field resets (resumes); allocated
    // when a read is requested, so detection doesn't carry it.
    uint8_t* progress;
    uint16_t done_count;
    uint16_t resume_done; // done_count at the last resume
    uint8_t resumes; // field resets since the read last made progress
    uint8_t setup_attempts; // failed handshakes in a row
    bool rx_reduced_gain; // receiver gain for the next handshake or session

    // Write request: chained WRITEs, resumed across sessions.
    uint16_t write_offset;
    const uint8_t* write_data;
    uint16_t write_size;
    uint16_t write_pos;
    uint8_t write_attempts;
    uint8_t write_uid[LEGIC_PRIME_UID_SIZE]; // read by the first write session
    bool write_uid_known;
    uint8_t write_ack_window; // bit window of the last write ACK (predicts the next)

    NfcGenericEvent general_event;
    LegicPrimePollerEvent legic_event;
    LegicPrimePollerEventData legic_event_data;
    NfcGenericCallback callback;
    void* context;
};

const LegicPrimeData* legic_prime_poller_get_data(LegicPrimePoller* instance);

/**
 * Builds one reader frame on the running keystream: forwards `pre_steps` (the
 * tag's idle steps since the previous frame or reply), XORs the frame bits,
 * then, if a reply is expected, skips the tag's LEGIC_PRIME_REPLY_STEPS and
 * keeps the reply's keystream in ex->ks.
 */
void legic_prime_poller_build(
    LegicPrimeExchange* ex,
    uint32_t value,
    uint8_t bits,
    uint8_t pre_steps,
    uint8_t rx_bits);

/** Starts the keystream from `iv`: the IV frame (sent plain) and the
 *  keystream of the tag's type reply. */
void legic_prime_poller_build_iv(LegicPrimeExchange* ex, uint8_t iv);

/** WRITE frame before the keystream: the address with bit 0 clear, the data
 *  byte and a CRC-4 over both. */
uint32_t legic_prime_poller_write_frame(uint16_t addr, uint8_t value, uint8_t cmd_bits);

/** Detects the tag: sends the IV and decodes the card type into
 *  instance->data.  The tag then waits for an ACK, so reset the field
 *  before the next session. */
LegicPrimeError legic_prime_poller_setup(LegicPrimePoller* instance, uint8_t iv);

/**
 * One read session from a fresh field: handshake, then read the bytes not
 * read yet, within the session's exchange budget.  The read is complete once
 * done_count reaches data_size; until then reset the field and run another
 * session.
 *
 * @return LegicPrimeErrorNone unless the session failed.  Running out of
 * budget only fails a session if bytes read before came up again and none
 * of them could be confirmed (a MIM1024's first session reads each byte once).
 */
LegicPrimeError legic_prime_poller_read(LegicPrimePoller* instance);

// One write session from write_pos on: handshake, UID check, then WRITEs
// chained after the tag's ACKs.  Advances write_pos for every acknowledged
// byte; the session may end early (without an error) for the next one to
// continue.  LegicPrimeErrorWrongCard if the UID differs from the first
// session's.
LegicPrimeError legic_prime_poller_write_session(LegicPrimePoller* instance);

#ifdef __cplusplus
}
#endif
