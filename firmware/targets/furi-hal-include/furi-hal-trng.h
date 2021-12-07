#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Get random value
 *
 * @return     random value
 */
uint32_t furi_hal_trng_get_random();

/** Fill buffer with random data
 *
 * @param      buf  buffer pointer
 * @param      data buffer len
 */
void furi_hal_trng_fill_buf(uint8_t* buf, uint32_t len);

#ifdef __cplusplus
}
#endif