#include "iso14443_4a_i.h"

bool iso14443_4a_ats_parse(Iso14443_4aAtsData* data, const BitBuffer* buf) {
    // TODO better check
    const bool can_parse = bit_buffer_get_size_bytes(buf) == sizeof(Iso14443_4aAtsData);
    if(can_parse) {
        bit_buffer_write_bytes(buf, data, sizeof(Iso14443_4aAtsData));
    }
    return can_parse;
}
