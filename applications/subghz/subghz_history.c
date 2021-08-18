#include "subghz_history.h"
#include "../../lib/subghz/protocols/subghz_protocol_keeloq.h"
#include "../../lib/subghz/protocols/subghz_protocol_star_line.h"
#include "../../lib/subghz/protocols/subghz_protocol_princeton.h"

//#include "subghz_protocol.h"
// #include "subghz_protocol_came.h"
// #include "subghz_protocol_cfm.h"

// #include "subghz_protocol_nice_flo.h"
// #include "subghz_protocol_nice_flor_s.h"
// #include "subghz_protocol_princeton.h"
// #include "subghz_protocol_gate_tx.h"
// #include "subghz_protocol_ido.h"
// #include "subghz_protocol_faac_slh.h"
// #include "subghz_protocol_nero_sketch.h"
//#include "subghz_protocol_star_line.h"

#include <furi.h>
#include <m-string.h>

#define SUBGHZ_HISTORY_MAX 20

typedef struct SubGhzHistoryStruct SubGhzHistoryStruct;

struct SubGhzHistoryStruct {
    const char* name;
    string_t manufacture_name;
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
    uint8_t last_index_write;
    SubGhzHistoryStruct history[SUBGHZ_HISTORY_MAX];
};

SubGhzHistory* subghz_history_alloc(void) {
    SubGhzHistory* instance = furi_alloc(sizeof(SubGhzHistory));
    for(uint8_t i = 0; i < SUBGHZ_HISTORY_MAX; i++) {
        string_init(instance->history[i].manufacture_name);
    }
    return instance;
}

void subghz_history_free(SubGhzHistory* instance) {
    furi_assert(instance);
    for(uint8_t i = 0; i < SUBGHZ_HISTORY_MAX; i++) {
        string_clear(instance->history[i].manufacture_name);
    }
    free(instance);
}

static void subghz_history_add_to_history(SubGhzHistory* instance, void* context) {
    furi_assert(instance);
    furi_assert(context);
    SubGhzProtocolCommon* protocol = context;

    if(instance->last_index_write > SUBGHZ_HISTORY_MAX) return;
    if((instance->history[instance->last_index_write].code_found == protocol->code_last_found) &&
       ((instance->last_update_timestamp - millis()) < 500)) {
        return;
    }

    instance->last_update_timestamp = millis();
    instance->history[instance->last_index_write].te = 0;
    instance->history[instance->last_index_write].name = protocol->name;
    instance->history[instance->last_index_write].code_count_bit = protocol->code_last_count_bit;
    instance->history[instance->last_index_write].code_found = protocol->code_last_found;
    instance->history[instance->last_index_write].code_reverse_found =
        subghz_protocol_common_reverse_key(
            protocol->code_last_count_bit, protocol->code_last_found);
    if(strcmp(protocol->name, "KeeLoq") == 0) {
        string_set(
            instance->history[instance->last_index_write].manufacture_name,
            subghz_protocol_keeloq_get_manufacture_name(protocol));
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found >> 32;
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found & 0xFFFFFFFF;
    } else if(strcmp(protocol->name, "Star Line") == 0) {
        string_set(
            instance->history[instance->last_index_write].manufacture_name,
            subghz_protocol_star_line_get_manufacture_name(protocol));
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found >> 32;
        instance->history[instance->last_index_write].fix =
            instance->history[instance->last_index_write].code_reverse_found & 0xFFFFFFFF;
    } else if(strcmp(protocol->name, "Princeton") == 0) {
        instance->history[instance->last_index_write].te =
            subghz_protocol_princeton_get_te(protocol);
        string_set(instance->history[instance->last_index_write].manufacture_name, NULL);
        instance->history[instance->last_index_write].fix = 0;
        instance->history[instance->last_index_write].fix = 0;
    } else {
        string_set(instance->history[instance->last_index_write].manufacture_name, NULL);
        instance->history[instance->last_index_write].fix = 0;
        instance->history[instance->last_index_write].fix = 0;
    } 
    instance->history[instance->last_index_write].serial = protocol->serial;
    instance->history[instance->last_index_write].type_protocol = protocol->type_protocol;
    instance->history[instance->last_index_write].cnt = protocol->cnt;
    instance->history[instance->last_index_write].btn = protocol->btn;

    instance->last_index_write++;
}