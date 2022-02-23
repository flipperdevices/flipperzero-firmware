#include "subghz_history.h"
//#include <lib/subghz/protocols/subghz_protocol_keeloq.h>
//#include <lib/subghz/protocols/subghz_protocol_star_line.h>
//#include <lib/subghz/protocols/subghz_protocol_princeton.h>
// #include <lib/subghz/protocols/subghz_protocol_somfy_keytis.h>
#include <lib/subghz/receiver.h>
#include <lib/flipper_format/flipper_format.h>
#include <lib/flipper_format/flipper_format_stream.h>
#include <lib/subghz/protocols/came.h>

#include <furi.h>
#include <m-string.h>

#define SUBGHZ_HISTORY_MAX 50
#define TAG "SubGhzHistory"

typedef struct {
    string_t item_str;
    FlipperFormat* flipper_string;
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
    uint8_t code_last_hash_data;
    string_t tmp_string;
    //FlipperFormat* flipper_format;
    //Stream* stream;
    SubGhzHistoryStruct* history;
    //SubGhzProtocolCommonLoad data;
};

SubGhzHistory* subghz_history_alloc(void) {
    SubGhzHistory* instance = malloc(sizeof(SubGhzHistory));
    string_init(instance->tmp_string);
    //instance->flipper_format = flipper_format_string_alloc();

    instance->history = malloc(sizeof(SubGhzHistoryStruct));
    SubGhzHistoryItemArray_init(instance->history->data);
    return instance;
}

void subghz_history_free(SubGhzHistory* instance) {
    furi_assert(instance);
    //flipper_format_free(instance->flipper_format);
    string_clear(instance->tmp_string);
    for
        M_EACH(item, instance->history->data, SubGhzHistoryItemArray_t) {
            string_clear(item->item_str);
            flipper_format_free(item->flipper_string);
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
    string_reset(instance->tmp_string);
    for
        M_EACH(item, instance->history->data, SubGhzHistoryItemArray_t) {
            string_clear(item->item_str);
            flipper_format_free(item->flipper_string);
            item->type = 0;
        }
    SubGhzHistoryItemArray_reset(instance->history->data);
    instance->last_index_write = 0;
    instance->code_last_hash_data = 0;
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

const char* subghz_history_get_protocol_name(SubGhzHistory* instance, uint16_t idx) {
    furi_assert(instance);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
    flipper_format_rewind(item->flipper_string);
    if(!flipper_format_read_string(item->flipper_string, "Protocol", instance->tmp_string)) {
        FURI_LOG_E(TAG, "Missing Protocol");
        string_reset(instance->tmp_string);
    }
    return string_get_cstr(instance->tmp_string);
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
}

// void subghz_history_get_text_decoder(SubGhzHistory* instance, string_t output, uint16_t idx) {
//     SubGhzHistoryItem* item = SubGhzHistoryItemArray_get(instance->history->data, idx);
//     if(flipper_format_read_string(item->flipper_string, "Protocol", instance->tmp_string)) {
//         subghz_protocol_decoder_base_serialize( subghz_protocol_decoder, text);

//         FURI_LOG_E(TAG, "Missing Protocol");
//         string_reset(instance->tmp_string);
//     }

//     string_set(output, item->item_str);
// }

bool subghz_history_add_to_history(
    SubGhzHistory* instance,
    void* context,
    uint32_t frequency,
    FuriHalSubGhzPreset preset) {
    furi_assert(instance);
    furi_assert(context);

    if(instance->last_index_write >= SUBGHZ_HISTORY_MAX) return false;

    SubGhzProtocolDecoderBase* decoder_base = context;
    if((instance->code_last_hash_data ==
        subghz_protocol_decoder_base_get_hash_data(decoder_base)) &&
       ((millis() - instance->last_update_timestamp) < 500)) {
        instance->last_update_timestamp = millis();
        return false;
    }

    instance->code_last_hash_data = subghz_protocol_decoder_base_get_hash_data(decoder_base);
    instance->last_update_timestamp = millis();

    string_t text;
    string_init(text);
    SubGhzHistoryItem* item = SubGhzHistoryItemArray_push_raw(instance->history->data);
    item->type = decoder_base->protocol->type;
    item->frequency = frequency;
    item->preset = preset;

    string_init(item->item_str);
    item->flipper_string = flipper_format_string_alloc();
    subghz_protocol_decoder_base_serialize(decoder_base, item->flipper_string, frequency, preset);

    do {
        if(!flipper_format_rewind(item->flipper_string)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        if(!flipper_format_read_string(item->flipper_string, "Protocol", instance->tmp_string)) {
            FURI_LOG_E(TAG, "Missing Protocol");
            break;
        }
        if(!strcmp(string_get_cstr(instance->tmp_string), "KeeLoq")) {
            string_set(instance->tmp_string, "KL ");
            if(!flipper_format_read_string(item->flipper_string, "Manufacture", text)) {
                FURI_LOG_E(TAG, "Missing Protocol");
                break;
            }
            string_cat(instance->tmp_string, text);
        } else if(!strcmp(string_get_cstr(instance->tmp_string), "Star Line")) {
            string_set(instance->tmp_string, "SL ");
            if(!flipper_format_read_string(item->flipper_string, "Manufacture", text)) {
                FURI_LOG_E(TAG, "Missing Protocol");
                break;
            }
            string_cat(instance->tmp_string, text);
        }
        if(!flipper_format_rewind(item->flipper_string)) {
            FURI_LOG_E(TAG, "Rewind error");
            break;
        }
        uint8_t key_data[sizeof(uint64_t)] = {0};
        if(!flipper_format_read_hex(item->flipper_string, "Key", key_data, sizeof(uint64_t))) {
            FURI_LOG_E(TAG, "Missing Key");
            break;
        }
        uint64_t data;
        for(uint8_t i = 0; i < sizeof(uint64_t); i++) {
            data = (data << 8) | key_data[i];
        }
        if(!(uint32_t)(data >> 32)) {
            string_printf(
                item->item_str,
                "%s %lX",
                string_get_cstr(instance->tmp_string),
                (uint32_t)(data & 0xFFFFFFFF));
        } else {
            string_printf(
                item->item_str,
                "%s %lX%08lX",
                string_get_cstr(instance->tmp_string),
                (uint32_t)(data >> 32),
                (uint32_t)(data & 0xFFFFFFFF));
        }
    } while(false);
    string_clear(text);

    //string_init_set_str(item->item_str, string_get_cstr(text));

    //printf("%s", string_get_cstr(text));

    // subghz_protocol_decoder_base_serialize(decoder_base, text);

    // //subghz_protocol_came_save_file(decoder_base, instance->flipper_format, frequency, preset);
    // subghz_protocol_came_load_file(decoder_base, instance->flipper_format, "");

    // SubGhzHistoryItem* item = SubGhzHistoryItemArray_push_raw(instance->history->data);
    // item->flipper_format = flipper_format_string_alloc();
    // string_init_set_str(item->item_str, string_get_cstr(text));
    // item->type = decoder_base->protocol->type;

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
