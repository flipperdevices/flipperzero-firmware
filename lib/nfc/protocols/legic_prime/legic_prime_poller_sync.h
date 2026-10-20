#pragma once

#include "legic_prime.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Reads the whole card.  Each byte needs two agreeing reads, as a 4-bit CRC
 * passes 1 in 16 misdecoded replies; `data` is only filled on success.
 */
LegicPrimeError legic_prime_poller_sync_read(Nfc* nfc, LegicPrimeData* data);

/**
 * Reads `size` bytes (1 to LEGIC_PRIME_MEM_MAX) from `offset`, clamped to
 * the end of the card.  As for legic_prime_poller_sync_read(), each byte
 * needs two agreeing reads and `data` is only filled on success: the card
 * type and size, the UID and the bytes of the range, the others 0, so it is
 * no dump to save.
 *
 * @param[out] bytes_read bytes of the range confirmed (may be NULL), on
 * success and on failure.
 * @param[out] bytes_total size of the range clamped to the card (may be
 * NULL); 0 if the card was not detected.
 */
LegicPrimeError legic_prime_poller_sync_read_range(
    Nfc* nfc,
    uint16_t offset,
    uint16_t size,
    LegicPrimeData* data,
    uint16_t* bytes_read,
    uint16_t* bytes_total);

/**
 * Writes `size` bytes (1 to LEGIC_PRIME_MEM_MAX) from `offset` (>= 5: MCD,
 * MSN and MCC are read-only).  Bytes are written last to first, several per
 * session, each WRITE chained ~240 µs behind the tag's ACK of the previous
 * one; the write resumes in a new session (new field, new handshake, UID
 * re-check) whenever an ACK lands outside the predicted bit window, and
 * after 256 bytes.
 *
 * Success means the tag acknowledged every byte: its ACK is a single
 * unencrypted bit, as Proxmark3 takes it.  To be sure the card holds the
 * data, read it back with legic_prime_poller_sync_read_range().
 *
 * @param[out] bytes_written bytes the tag acknowledged (may be NULL): the
 * last ones of the range, on success and on failure.
 * @param[out] write_unacked set on failure if the WRITE of the byte just
 * before the acknowledged ones went out without an ACK, so that byte may
 * hold the new value (may be NULL).
 *
 * @warning Bytes 5-6 are the DCF, which a real tag only lets decrease:
 * writing them is irreversible.  Bytes 7-8 mark a card as segmented.
 */
LegicPrimeError legic_prime_poller_sync_write(
    Nfc* nfc,
    uint16_t offset,
    const uint8_t* data,
    uint16_t size,
    uint16_t* bytes_written,
    bool* write_unacked);

#ifdef __cplusplus
}
#endif
