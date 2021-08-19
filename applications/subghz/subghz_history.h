#pragma once

#include <lib/subghz/protocols/subghz_protocol_common.h>

typedef struct SubGhzHistory SubGhzHistory;

SubGhzHistory* subghz_history_alloc(void);
void subghz_history_free(SubGhzHistory* instance);
void subghz_history_clean(SubGhzHistory* instance);
uint16_t subghz_history_get_item(SubGhzHistory* instance);
uint8_t subghz_history_get_type_protocol(SubGhzHistory* instance, uint16_t idx);
const char* subghz_history_get_name(SubGhzHistory* instance, uint16_t idx);
void subghz_history_get_text_item_menu(SubGhzHistory* instance, string_t output, uint16_t idx);
void subghz_history_add_to_history(SubGhzHistory* instance, void* context);
