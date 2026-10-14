/**
 * @file legic_prime_crc.h
 * @brief CRC helpers for LEGIC Prime.
 *
 * Two CRCs are used:
 *  - 4-bit per-frame CRC validating reader command + tag data byte.
 *    Polynomial 0x19 >> 1 = 0x0C (right-shift / LSB-first form), init 0x05.
 *    Reflected variant of poly 0x09 (x^4 + x^3 + 1).
 *  - 8-bit MCC over UID bytes (MCD + MSN). Polynomial 0x63, init 0x55,
 *    reflected input and output.
 *
 * Polynomials and parameters come from Proxmark3 (armsrc/legicrf.c:313,
 * common/crc.c CRC8Legic).
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
 * @param cmd_sz command size in bits (6/9/11 for MIM22/256/1024 read; +9 for write).
 * @param value data byte to include in the CRC (use 0 for command-only CRC).
 * @returns 4-bit CRC value.
 */
uint8_t legic_prime_crc4(uint32_t cmd, uint8_t cmd_sz, uint8_t value);

/**
 * Compute the LEGIC MCC (CRC-8) over the first 4 UID bytes (MCD + MSN).
 *
 * @param buf pointer to buffer holding at least @p size bytes.
 * @param size number of bytes to feed (4 for header MCC).
 * @returns 8-bit CRC.
 */
uint8_t legic_prime_crc8(const uint8_t* buf, size_t size);

#ifdef __cplusplus
}
#endif
