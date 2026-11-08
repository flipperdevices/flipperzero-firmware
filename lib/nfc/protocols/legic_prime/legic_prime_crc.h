/**
 * @file legic_prime_crc.h
 * @brief CRC helpers for LEGIC Prime.
 *
 * Two CRCs are used:
 *  - 4-bit per-frame CRC validating reader command + tag data byte.
 *    Polynomial 0x19 >> 1 = 0x0C (right-shift / LSB-first form of
 *    x^4 + x + 1), init 0x05.
 *  - CRC-8/LEGIC (MCC and segment header CRCs). Polynomial 0x63, init 0x55,
 *    reflected input and output.
 *
 * Parameters as in Proxmark3: crc_init(&legic_crc, 4, 0x19 >> 1, 0x05, 0) in
 * armsrc/legicrf.c init_reader(), and CRC8Legic() in common/crc.c.
 *
 * Copyright (C) Proxmark3 contributors. See AUTHORS.md for details
 * (https://github.com/RfidResearchGroup/proxmark3).
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Compute the per-frame CRC-4.
 *
 * @param cmd full command word (address | opcode), as transmitted on the air.
 * @param cmd_sz command size in bits (6/9/11 for MIM22/256/1024), the same
 * for READ and WRITE; the data byte is passed in @p value.
 * @param value data byte to include in the CRC.
 * @returns 4-bit CRC value.
 */
uint8_t legic_prime_crc4(uint32_t cmd, uint8_t cmd_sz, uint8_t value);

/**
 * Compute CRC-8/LEGIC over @p size bytes: the 4 UID bytes (MCD + MSN) for the
 * MCC, the UID and a segment header for a segment CRC.
 *
 * @param buf pointer to buffer holding at least @p size bytes.
 * @param size number of bytes to feed.
 * @returns 8-bit CRC.
 */
uint8_t legic_prime_crc8(const uint8_t* buf, size_t size);

#ifdef __cplusplus
}
#endif
