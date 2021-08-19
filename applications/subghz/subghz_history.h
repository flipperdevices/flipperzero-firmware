#pragma once

#include "../../lib/subghz/protocols/subghz_protocol_common.h"

typedef struct SubGhzHistory SubGhzHistory;

SubGhzHistory* subghz_history_alloc(void);
void subghz_history_free(SubGhzHistory* instance);
void subghz_history_clean(SubGhzHistory* instance);
uint16_t subghz_history_get_item(SubGhzHistory* instance);
const char* subghz_history_get_name(SubGhzHistory* instance, uint16_t idx);
void subghz_history_add_to_history(SubGhzHistory* instance, void* context);
