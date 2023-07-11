#pragma once

#include "iso14443_4a.h"

typedef struct {
    uint8_t tl;
    uint8_t t0;
    uint8_t ta_1;
    uint8_t tb_1;
    uint8_t tc_1;
    uint8_t t1_tk[];
} Iso14443_4aAtsData;

bool iso14443_4a_ats_parse(SimpleArray* data, const BitBuffer* buf);

void iso14443_4a_ats_fill_default(SimpleArray* data);

Iso14443_4aError iso14443_4a_process_error(Iso14443_3aError error);
