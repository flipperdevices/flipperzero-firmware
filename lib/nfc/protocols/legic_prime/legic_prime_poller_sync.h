#pragma once

#include "legic_prime.h"
#include <nfc/nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

LegicPrimeError legic_prime_poller_sync_read(Nfc* nfc, LegicPrimeData* data);

/**
 * Writes `size` bytes from `offset` (>= 5: MCD, MSN and MCC are read-only).
 * Each byte is written in its own session and acknowledged by the tag.
 *
 * @warning Bytes 5-6 are the DCF, which a real tag only lets decrease:
 * writing them is irreversible.  Bytes 7-8 mark a card as segmented.
 */
LegicPrimeError
    legic_prime_poller_sync_write(Nfc* nfc, uint16_t offset, const uint8_t* data, size_t size);

#ifdef __cplusplus
}
#endif
