#pragma once

#include "rolling_flaws_structs.h"

#define FAILED_TO_PARSE 0x0BADC0DE

void decode_keeloq(RollingFlawsModel* model, FuriString* buffer, bool sync);