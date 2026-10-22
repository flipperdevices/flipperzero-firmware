#pragma once

#include "legic_prime.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Both calls block until the poller reports success or failure: there is
 * no timeout and no abort.  The poller goes on in a new session (new field,
 * new handshake, UID re-check) as long as it makes progress, within a total
 * session budget (legic_prime_poller_max_sessions(), internal to the
 * poller: 32 times the sessions a clean run needs, plus 8), so a weakly
 * coupled whole-card MIM1024 read can take minutes.  An application that
 * needs an abort drives the generic poller as `nfc dump` does:
 * nfc_poller_alloc(nfc, NfcProtocolLegicPrime), nfc_poller_start() with a
 * callback that answers LegicPrimePollerEventTypeRequestMode, and
 * nfc_poller_stop(), which returns once the running session ends (at most
 * ~2 s) and reports no result.
 */

/**
 * Reads `size` bytes (1 to LEGIC_PRIME_MEM_MAX) from `offset`, clamped to
 * the end of the card; offset 0 and size LEGIC_PRIME_MEM_MAX read the whole
 * card, whatever its size.  A range that starts at or past the end fails
 * with LegicPrimeErrorReadOnly.  Each byte needs two agreeing reads, as a
 * 4-bit CRC passes 1 in 16 misdecoded replies.  `data` is only filled on
 * success: the card type, the UID and the bytes of the range, the others
 * 0, so only a whole-card read gives a dump to save.  Blocks, see the top
 * of this header.
 *
 * @param[out] bytes_read bytes of the range confirmed (may be NULL), on
 * success and on failure.
 * @param[out] bytes_total size of the range clamped to the card (may be
 * NULL); 0 if the card was not detected, or if the range starts at or past
 * the card's end (LegicPrimeErrorReadOnly once a session confirmed the
 * UID, another error if none did).
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
 * after 256 bytes.  Blocks, see the top of this header.
 *
 * Success means the tag acknowledged every byte: its ACK is a single
 * unencrypted bit, as Proxmark3 takes it.  To be sure the card holds the
 * data, read it back with legic_prime_poller_sync_read_range().
 *
 * @param[out] bytes_written bytes the tag acknowledged (may be NULL): the
 * last ones of the range, on success and on failure.
 * @param[out] write_unacked set on failure if the WRITE of the byte just
 * before the acknowledged ones went out without an ACK, so that byte may
 * hold the new value (may be NULL).  A WRITE chained behind that missing
 * ACK may have gone out as well, so the byte before it is unconfirmed too;
 * read the range back to be sure.
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
