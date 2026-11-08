/**
 * @file furi_hal_nfc_legic_i.h
 * @brief LEGIC Prime poller arithmetic shared with the unit tests.
 *
 * Private to the HAL (*_i.h is not an SDK header).  nfc_test.c
 * legic_prime_write_chain_steps_test runs this on legicrfsim.c's ticks.
 */
#pragma once

#include <stdint.h>

// legicrfsim.c's rx_frame() steps the tag 3 times for a frame whose first
// pause comes MIN_TICKS to MAX_TICKS of its 4.72 µs ticks after the ACK's
// end: nfc_test.c legic_prime_write_chain_steps_test asserts both edges in
// its model of rx_frame(), and furi_hal_nfc_legic.c asserts that its
// post-ACK gap lies inside.
#define FURI_HAL_NFC_LEGIC_SIM_3STEP_MIN_TICKS (41U)
#define FURI_HAL_NFC_LEGIC_SIM_3STEP_MAX_TICKS (61U)

/**
 * @brief Bit window of a reply run, counted like reply bits.
 *
 * @param start run start minus the start of window 0
 * @param bit   one window in the same unit (CPU cycles in the HAL,
 *              212 kHz ticks in nfc_test.c)
 * @return the window whose start is nearest, -1 if the run starts more
 *         than half a window before window 0
 */
static inline int32_t furi_hal_nfc_legic_round_window(int32_t start, int32_t bit) {
    return start < -bit / 2 ? -1 : (start + bit / 2) / bit;
}
