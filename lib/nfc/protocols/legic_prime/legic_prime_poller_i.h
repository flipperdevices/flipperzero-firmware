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

struct LegicPrimePoller {
    Nfc* nfc;
    LegicPrimePollerState state;
    LegicPrimeData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    // PRNG bookkeeping: cmd_size and addr_size cached from the detected type.
    uint8_t cmd_size_bits;

    // Read progress, kept across field resets (resumes).
    uint8_t done[LEGIC_PRIME_MEM_MAX / 8];
    // Read once with a valid CRC; done once a second read agrees.
    uint8_t seen[LEGIC_PRIME_MEM_MAX / 8];
    uint8_t attempts[LEGIC_PRIME_MEM_MAX];
    uint16_t done_count;
    uint16_t resume_done; // done_count at the last resume
    uint8_t resumes; // field resets since the read last made progress
    uint8_t setup_attempts; // failed handshakes in a row
    bool rx_full_gain; // receiver gain for the next handshake or session

    // Write mode request (one byte per session).
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

// One write session from write_pos on: handshake, UID check, then WRITEs
// chained after the tag's ACKs.  Advances write_pos for every acknowledged
// byte; the session may end early (without an error) for the next one to
// continue.  LegicPrimeErrorWrongCard if the UID differs from the first
// session's.
LegicPrimeError legic_prime_poller_write_session(LegicPrimePoller* instance);

#ifdef __cplusplus
}
#endif
