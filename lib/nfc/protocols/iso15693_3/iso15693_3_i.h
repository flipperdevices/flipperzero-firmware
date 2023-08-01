#pragma once

#include "iso15693_3.h"

#include <nfc/helpers/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Check if the buffer contains an error frame and if it does, determine
 * the error type.
 * NOTE: No changes are done to the result if no error is present.
 *
 * @param [out] data Pointer to the resulting error value.
 * @param [in] buf Data buffer to be checked
 *
 * @return True if data contains an error frame or is empty, false otherwise
 */
bool iso15693_3_error_response_parse(Iso15693_3Error* error, const BitBuffer* buf);

Iso15693_3Error iso15693_3_inventory_response_parse(uint8_t* data, const BitBuffer* buf);

Iso15693_3Error
    iso15693_3_system_info_response_parse(Iso15693_3SystemInfo* data, const BitBuffer* buf);

Iso15693_3Error
    iso15693_3_read_block_response_parse(uint8_t* data, uint8_t block_size, const BitBuffer* buf);

Iso15693_3Error iso15693_3_get_block_security_response_parse(
    uint8_t* data,
    uint16_t block_count,
    const BitBuffer* buf);

#ifdef __cplusplus
}
#endif
