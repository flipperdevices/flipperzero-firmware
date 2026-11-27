#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Tag timing in carrier cycles (fc, 73.7 ns).  A chip is one 212 kHz
// subcarrier period (64 fc, 4.72 µs), the tick Proxmark uses.
#define LEGIC_PRIME_LISTENER_CHIP_FC        (64U)
#define LEGIC_PRIME_LISTENER_BIT_CHIPS      (21U) // 99.1 µs
#define LEGIC_PRIME_LISTENER_BIT_FC         (LEGIC_PRIME_LISTENER_BIT_CHIPS * LEGIC_PRIME_LISTENER_CHIP_FC)
// legicrfsim.c counts the slots from 2 chips (TX + RX pipeline delay) before
// the reference point.
#define LEGIC_PRIME_LISTENER_PREROLL_FC     (2U * LEGIC_PRIME_LISTENER_CHIP_FC)
// A real tag loses power when the reader's field goes off, which ends its
// session.  The field isn't monitored here, so a frame this long after the
// reference starts a new session instead; catching the keystream up over a
// gap of seconds would also take far longer than the 330 µs reply slot.
// (Readers continue a session within a few hundred µs.)
#define LEGIC_PRIME_LISTENER_SESSION_GAP_FC (20U * 13560U) // 20 ms

/**
 * @brief Count the keystream steps a LEGIC Prime tag takes before a reader frame.
 *
 * While waiting for the reader, the tag steps its keystream once per 99.1 µs
 * slot, counted from 2 chips before the reference point (the end of its last
 * reply, or one slot after the reader's ACK), and at least once: Proxmark3
 * armsrc/legicrfsim.c rx_frame().
 *
 * @param[in] elapsed_fc carrier cycles from the reference point to the frame's first pause.
 * @returns number of steps, or 0 if the frame came too late to belong to the session.
 */
static inline uint32_t legic_prime_listener_idle_steps(uint32_t elapsed_fc) {
    if(elapsed_fc > LEGIC_PRIME_LISTENER_SESSION_GAP_FC) return 0;
    return (elapsed_fc + LEGIC_PRIME_LISTENER_PREROLL_FC + LEGIC_PRIME_LISTENER_BIT_FC - 1U) /
           LEGIC_PRIME_LISTENER_BIT_FC;
}

#ifdef __cplusplus
}
#endif
