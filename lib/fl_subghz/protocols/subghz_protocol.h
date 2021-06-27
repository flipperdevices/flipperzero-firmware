#pragma once

#include "subghz_protocol_common.h"

typedef struct SubGhzProtocol SubGhzProtocol;

SubGhzProtocol* subghz_protocol_alloc();

void subghz_protocol_free(SubGhzProtocol* instance);

void subghz_protocol_reset(SubGhzProtocol* instance);

void subghz_protocol_parse(SubGhzProtocol* instance, LevelPair data);
