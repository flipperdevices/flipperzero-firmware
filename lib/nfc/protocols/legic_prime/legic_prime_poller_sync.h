#pragma once

#include "legic_prime.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Reads the whole card. */
LegicPrimeError legic_prime_poller_sync_read(Nfc* nfc, LegicPrimeData* data);

/**
 * Writes `size` bytes from `offset` (>= 5: MCD, MSN and MCC are read-only).
 * Bytes are written last to first, several per session, each WRITE chained
 * ~240 µs behind the tag's ACK of the previous one; the write resumes in a
 * new session (new field, new handshake, UID re-check) whenever an ACK lands
 * outside the predicted bit window.
 *
 * @param[out] bytes_written bytes the tag acknowledged (may be NULL): the
 * last ones of the range, on success and on failure.
 *
 * @warning Bytes 5-6 are the DCF, which a real tag only lets decrease:
 * writing them is irreversible.  Bytes 7-8 mark a card as segmented.
 */
LegicPrimeError legic_prime_poller_sync_write(
    Nfc* nfc,
    uint16_t offset,
    const uint8_t* data,
    size_t size,
    size_t* bytes_written);

#ifdef __cplusplus
}
#endif
