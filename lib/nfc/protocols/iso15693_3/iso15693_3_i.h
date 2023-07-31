#pragma once

#include "iso15693_3.h"

#include <nfc/helpers/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

bool iso15693_3_inventory_response_parse(Iso15693_3Data* data, const BitBuffer* buf);

bool iso15693_3_system_info_response_parse(Iso15693_3Data* data, const BitBuffer* buf);

bool iso15693_3_read_block_response_parse(uint8_t* data, size_t data_size, const BitBuffer* buf);

#ifdef __cplusplus
}
#endif
