#pragma once

#include <furi.h>

#include "subghz_keystore.h"

typedef struct SubGhzEnvironment SubGhzEnvironment;

SubGhzEnvironment* subghz_environment_alloc();

void subghz_environment_free(SubGhzEnvironment* instance);

void subghz_environment_load_keystore(SubGhzEnvironment* instance, const char* filename);

SubGhzKeystore* subghz_environment_get_keystore(SubGhzEnvironment* instance);
