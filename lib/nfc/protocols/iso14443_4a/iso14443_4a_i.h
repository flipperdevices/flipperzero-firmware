#pragma once

#include "iso14443_4a.h"

bool iso14443_4a_ats_parse(Iso14443_4aAtsData* data, const BitBuffer* buf);

void iso14443_4a_ats_fill_default(Iso14443_4aAtsData* data);
