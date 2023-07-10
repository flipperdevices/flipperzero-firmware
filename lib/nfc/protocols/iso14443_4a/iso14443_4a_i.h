#pragma once

#include "iso14443_4a.h"

bool iso14443_4a_ats_parse(Iso14443_4aAtsData* data, const BitBuffer* buf);

void iso14443_4a_ats_fill_default(Iso14443_4aAtsData* data);

Iso14443_4aError iso14443_4a_process_error(Iso14443_3aError error);
