#pragma once

#include <core/string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Format a data buffer as a HEX string with line breaks
 * @param [out] result pointer to the output string (must be initialised)
 * @param [in] row_length row length (in HEX bytes) before line break
 * @param [in] data pointer to the input data buffer
 * @param [in] data_size input data size
 */
void pretty_format_bytes_hex(
    FuriString* result,
    size_t row_length,
    const uint8_t* data,
    size_t data_size);

/**
 * Format a data buffer as an ASCII string with line breaks
 * @param [out] result pointer to the output string (must be initialised)
 * @param [in] row_length row length (in ASCII characters) before line break
 * @param [in] data pointer to the input data buffer
 * @param [in] data_size input data size
 */
void pretty_format_bytes_ascii(
    FuriString* result,
    size_t row_length,
    const uint8_t* data,
    size_t data_size);

#ifdef __cplusplus
}
#endif
