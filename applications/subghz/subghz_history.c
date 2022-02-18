#include "subghz_history.h"
//#include <lib/subghz/protocols/subghz_protocol_keeloq.h>
//#include <lib/subghz/protocols/subghz_protocol_star_line.h>
//#include <lib/subghz/protocols/subghz_protocol_princeton.h>
// #include <lib/subghz/protocols/subghz_protocol_somfy_keytis.h>
#include <lib/subghz/receiver.h>

#include <furi.h>
#include <m-string.h>

#define SUBGHZ_HISTORY_MAX 50

typedef struct {
    string_t item_str;
    uint8_t type;
    FuriHalSubGhzPreset preset;
    uint32_t frequency;
} SubGhzHistoryItem;

ARRAY_DEF(SubGhzHistoryItemArray, SubGhzHistoryItem, M_POD_OPLIST)

#define M_OPL_SubGhzHistoryItemArray_t() ARRAY_OPLIST(SubGhzHistoryItemArray, M_POD_OPLIST)

typedef struct {
    SubGhzHistoryItemArray_t data;
} SubGhzHistoryStruct;

//typedef struct SubGhzHistoryStruct SubGhzHistoryStruct;

// typedef struct {
//     const char* name;
//     const char* manufacture_name;
//     uint8_t type_protocol;
//     uint8_t code_count_bit;
//     uint64_t code_found;
//     uint32_t data1;
//     FuriHalSubGhzPreset preset;
//     uint32_t real_frequency;
// } SubGhzHistoryStruct;

struct SubGhzHistory {
    uint32_t last_update_timestamp;
    uint16_t last_index_write;
    uint64_t code_last_found;
    SubGhzHistoryStruct* history;
    //SubGhzProtocolCommonLoad data;
};

SubGhzHistory* subghz_history_alloc(void) {
    SubGhzHistory* instance = furi_alloc(sizeof(SubGhzHistory));
    instance->history = furi_alloc(sizeof(SubGhzHistoryStruct));
    SubGhzHistoryItemArray_init(instance->history->data);
    return instance;
}

void subghz_history_free(SubGhzHistory* instance) {
    furi_assert(instance);
    for
        M_EACH(item, instance->history->data, SubGhzHistoryItemArray_t) {
            string_clear(item->item_str);
            item->type = 0;
        }
    SubGhzHistoryItemArray_clear(instance->history->data);
    free(instance->history);
    free(instance);
}

// void subghz_history_set_frequency_preset(
//     SubGhzHistory* instance,
//     uint16_t idx,
//     uint32_t frequency,
//     FuriHalSubGhzPreset preset) {
//     furi_assert(instance);
//     if(instance->last_index_write >= SUBGHZ_HISTORY_MAX) return;
//     instance->history[idx].preset = preset;
//     instance->history[idx].real_frequency = frequency;
// }

uint32_t subghz_history_get_frequency(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    return item->frequency;
}

FuriHalSubGhzPreset subghz_history_get_preset(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    return item->preset;
}

void subghz_history_reset(SubGhzHistory* instance) {
    furi_assert(instance);
    for
        M_EACH(item, instance->history->data, SubGhzHistoryItemArray_t) {
            string_clear(item->item_str);
            item->type = 0;
        }
    SubGhzHistoryItemArray_reset(instance->history->data);
    instance->last_index_write = 0;
    instance->code_last_found = 0;
}

uint16_t subghz_history_get_item(SubGhzHistory* instance) {
    furi_assert(instance);
    return instance->last_index_write;
}

uint8_t subghz_history_get_type_protocol(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    return item->type;
}

const char* subghz_history_get_name(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    return string_get_cstr(item->item_str);
}

const char* subghz_history_get_raw_data(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    // instance->data.code_found = instance->history[idx].code_found;
    // instance->data.code_count_bit = instance->history[idx].code_count_bit;
    // instance->data.param1 = instance->history[idx].data1;
    // return &instance->data;
    return "TODO";
}
bool subghz_history_get_text_space_left(SubGhzHistory* instance, string_t output) {
    furi_assert(instance);
    if(instance->last_index_write == SUBGHZ_HISTORY_MAX) {
        if(output != NULL) string_printf(output, "Memory is FULL");
        return true;
    }
    if(output != NULL)
        string_printf(output, "%02u/%02u", instance->last_index_write, SUBGHZ_HISTORY_MAX);
    return false;
}
void subghz_history_get_text_item_menu(SubGhzHistory* instance, string_t output, uint16_t idx) {
    
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    string_set(output, item->item_str);

    // if(instance->history[idx].code_count_bit < 33) {
    //     string_printf(
    //         output,
    //         "%s %lX",
    //         instance->history[idx].name,
    //         (uint32_t)(instance->history[idx].code_found & 0xFFFFFFFF));
    // } else {
    //     string_t str_buff;
    //     string_init(str_buff);
    //     if(strcmp(instance->history[idx].name, "KeeLoq") == 0) {
    //         string_set(str_buff, "KL ");
    //         string_cat(str_buff, instance->history[idx].manufacture_name);
    //     } else if(strcmp(instance->history[idx].name, "Star Line") == 0) {
    //         string_set(str_buff, "SL ");
    //         string_cat(str_buff, instance->history[idx].manufacture_name);
    //     } else {
    //         string_set(str_buff, instance->history[idx].name);
    //     }

    //     string_printf(
    //         output,
    //         "%s %lX%08lX",
    //         string_get_cstr(str_buff),
    //         (uint32_t)(instance->history[idx].code_found >> 32),
    //         (uint32_t)(instance->history[idx].code_found & 0xFFFFFFFF));
    //     string_clear(str_buff);
    // }
}

bool subghz_history_add_to_history(
    SubGhzHistory* instance,
    void* context,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(instance);
    furi_assert(context);

    SubGhzProtocolDecoderBase* decoder_base = context;
    string_t text;
    string_init(text);
    subghz_protocol_decoder_base_serialize(decoder_base, text);

    SubGhzHistoryItem* item = SubGhzHistoryItemArray_push_raw(instance->history->data);
    string_init_set_str(item->item_str, string_get_cstr(text));
    item->type = decoder_base->protocol->type;
    item->frequency = frequency;
    item->preset = preset;
    printf("%s", string_get_cstr(text));
    string_clear(text);

    

    // if(instance->last_index_write >= SUBGHZ_HISTORY_MAX) return false;
    // if((instance->code_last_found == (protocol->code_last_found & 0xFFFF0FFFFFFFFFFF)) &&
    //    ((millis() - instance->last_update_timestamp) < 500)) {
    //     instance->last_update_timestamp = millis();
    //     return false;
    // }

    // instance->code_last_found = protocol->code_last_found & 0xFFFF0FFFFFFFFFFF;
    // instance->last_update_timestamp = millis();

    // instance->history[instance->last_index_write].real_frequency = frequency;
    // instance->history[instance->last_index_write].preset = preset;
    // instance->history[instance->last_index_write].data1 = 0;
    // instance->history[instance->last_index_write].manufacture_name = NULL;
    // instance->history[instance->last_index_write].name = protocol->name;
    // instance->history[instance->last_index_write].code_count_bit = protocol->code_last_count_bit;
    // instance->history[instance->last_index_write].code_found = protocol->code_last_found;
    // if(strcmp(protocol->name, "KeeLoq") == 0) {
    //     instance->history[instance->last_index_write].manufacture_name =
    //         subghz_protocol_keeloq_find_and_get_manufacture_name(protocol);
    // } else if(strcmp(protocol->name, "Star Line") == 0) {
    //     instance->history[instance->last_index_write].manufacture_name =
    //         subghz_protocol_star_line_find_and_get_manufacture_name(protocol);
    //} else
    // if(strcmp(protocol->name, "Princeton") == 0) {
    //     instance->history[instance->last_index_write].data1 =
    //         subghz_protocol_princeton_get_te(protocol);
    // }  else if(strcmp(protocol->name, "Somfy Keytis") == 0) {
    //     instance->history[instance->last_index_write].data1 =
    //         subghz_protocol_somfy_keytis_get_press_duration(protocol);
    // }

    //instance->history[instance->last_index_write].type_protocol = protocol->type_protocol;

    instance->last_index_write++;
    return true;
}
