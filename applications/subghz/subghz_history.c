#include "subghz_history.h"
#include <lib/subghz/protocols/subghz_protocol_keeloq.h>
#include <lib/subghz/protocols/subghz_protocol_star_line.h>
#include <lib/subghz/protocols/subghz_protocol_princeton.h>

#include <furi.h>
#include <m-string.h>

#define SUBGHZ_HISTORY_MAX 20

typedef struct SubGhzHistoryStruct SubGhzHistoryStruct;

struct SubGhzHistoryStruct {
    const char* name;
    const char* manufacture_name;
    uint8_t type_protocol;
    uint8_t code_count_bit;
    uint16_t cnt;
    uint64_t code_found;
    uint64_t code_reverse_found;
    uint32_t fix;
    uint32_t hop;
    uint32_t serial;
    uint8_t btn;
    uint16_t te;
};

struct SubGhzHistory {
    uint32_t last_update_timestamp;
    uint16_t last_index_write;
    uint64_t code_last_found;
    SubGhzHistoryStruct history[SUBGHZ_HISTORY_MAX];
};

SubGhzHistory* subghz_history_alloc(void) {
    SubGhzHistory* instance = furi_alloc(sizeof(SubGhzHistory));
    return instance;
}

void subghz_history_free(SubGhzHistory* instance) {
    furi_assert(instance);
    free(instance);
}
void subghz_history_clean(SubGhzHistory* instance) {
    furi_assert(instance);
    instance->last_index_write = 0;
    instance->code_last_found = 0;
}

uint16_t subghz_history_get_item(SubGhzHistory* instance) {
    furi_assert(instance);
    return instance->last_index_write;
}

uint8_t subghz_history_get_type_protocol(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    return instance->history[idx].type_protocol;
}

const char* subghz_history_get_name(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    return instance->history[idx].name;
}

void subghz_history_get_text_item_menu(SubGhzHistory* instance, string_t output, uint16_t idx) {
    if(instance->history[idx].code_count_bit < 33) {
        string_printf(
            output,
            "%s %lX",
            instance->history[idx].name,
            (uint32_t)(instance->history[idx].code_found & 0xFFFFFFFF));
    } else {
        string_t str_buff;
        string_init(str_buff);
        if(strcmp(instance->history[idx].name, "KeeLoq") == 0) {
            string_set(str_buff, "KL ");
            string_cat(str_buff, instance->history[idx].manufacture_name);
        } else if(strcmp(instance->history[idx].name, "Star Line") == 0) {
            string_set(str_buff, "SL ");
            string_cat(str_buff, instance->history[idx].manufacture_name);
        } else {
            string_set(str_buff, instance->history[idx].name);
        }

        string_printf(
            output,
            "%s %lX%08lX",
            string_get_cstr(str_buff),
            (uint32_t)(instance->history[idx].code_found >> 32),
            (uint32_t)(instance->history[idx].code_found & 0xFFFFFFFF));
        string_clean(str_buff);
    }
}

void subghz_history_add_to_history(SubGhzHistory* instance, void* context) {
    furi_assert(instance);
    furi_assert(context);
    SubGhzProtocolCommon* protocol = context;

    if(instance->last_index_write >= SUBGHZ_HISTORY_MAX) return;
    if((instance->code_last_found == (protocol->code_last_found & 0xFFFF0FFFFFFFFFFF)) &&
       ((millis() - instance->last_update_timestamp) < 500)) {
        instance->last_update_timestamp = millis();
        return;
    }

    instance->code_last_found = protocol->code_last_found & 0xFFFF0FFFFFFFFFFF;
    instance->last_update_timestamp = millis();

    instance->history[instance->last_index_write].te = 0;
    instance->history[instance->last_index_write].fix = 0;
    instance->history[instance->last_index_write].hop = 0;
    instance->history[instance->last_index_write].name = protocol->name;
    instance->history[instance->last_index_write].code_count_bit = protocol->code_last_count_bit;
    instance->history[instance->last_index_write].code_found = protocol->code_last_found;
    instance->history[instance->last_index_write].code_reverse_found =
        subghz_protocol_common_reverse_key(
            protocol->code_last_found, protocol->code_last_count_bit);
    if(strcmp(protocol->name, "KeeLoq") == 0) {
        instance->history[instance->last_index_write].manufacture_name =
            subghz_protocol_keeloq_get_manufacture_name(protocol);
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found >> 32;
        instance->history[instance->last_index_write].hop =
            instance->history[instance->last_index_write].code_reverse_found & 0xFFFFFFFF;
    } else if(strcmp(protocol->name, "Star Line") == 0) {
        instance->history[instance->last_index_write].manufacture_name =
            subghz_protocol_star_line_get_manufacture_name(protocol);
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found >> 32;
        instance->history[instance->last_index_write].hop =
            instance->history[instance->last_index_write].code_reverse_found & 0xFFFFFFFF;
    } else if(strcmp(protocol->name, "Princeton") == 0) {
        instance->history[instance->last_index_write].te =
            subghz_protocol_princeton_get_te(protocol);
        instance->history[instance->last_index_write].manufacture_name = NULL;
    } else {
        instance->history[instance->last_index_write].manufacture_name = NULL;
    }
    instance->history[instance->last_index_write].serial = protocol->serial;
    instance->history[instance->last_index_write].type_protocol = protocol->type_protocol;
    instance->history[instance->last_index_write].cnt = protocol->cnt;
    instance->history[instance->last_index_write].btn = protocol->btn;

    instance->last_index_write++;
}