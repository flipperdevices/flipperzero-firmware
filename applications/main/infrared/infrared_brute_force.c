#include "infrared_brute_force.h"

#include <stdlib.h>
#include <m-dict.h>
#include <core/furi_string.h>
#include <flipper_format/flipper_format.h>

#include "infrared_signal.h"

typedef struct {
    uint32_t index;
    uint32_t count;
} InfraredBruteForceRecord;

DICT_DEF2(
    InfraredBruteForceRecordDict,
    string_t,
    STRING_OPLIST,
    InfraredBruteForceRecord,
    M_POD_OPLIST);

struct InfraredBruteForce {
    FlipperFormat* ff;
    const char* db_filename;
    FuriString* current_record_name;
    InfraredBruteForceRecordDict_t records;
};

InfraredBruteForce* infrared_brute_force_alloc() {
    InfraredBruteForce* brute_force = malloc(sizeof(InfraredBruteForce));
    brute_force->ff = NULL;
    brute_force->db_filename = NULL;
    brute_force->current_record_name = furi_string_alloc();
    InfraredBruteForceRecordDict_init(brute_force->records);
    return brute_force;
}

void infrared_brute_force_free(InfraredBruteForce* brute_force) {
    furi_assert(!brute_force->ff);
    InfraredBruteForceRecordDict_clear(brute_force->records);
    furi_string_free(brute_force->current_record_name);
    free(brute_force);
}

void infrared_brute_force_set_db_filename(InfraredBruteForce* brute_force, const char* db_filename) {
    brute_force->db_filename = db_filename;
}

bool infrared_brute_force_calculate_messages(InfraredBruteForce* brute_force) {
    furi_assert(brute_force->db_filename);
    bool success = false;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    success = flipper_format_buffered_file_open_existing(ff, brute_force->db_filename);
    if(success) {
        FuriString* signal_name;
        signal_name = furi_string_alloc();
        while(flipper_format_read_string(ff, "name", signal_name)) {
            InfraredBruteForceRecord* record =
                InfraredBruteForceRecordDict_get(brute_force->records, signal_name);
            if(record) {
                ++(record->count);
            }
        }
        furi_string_free(signal_name);
    }

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool infrared_brute_force_start(
    InfraredBruteForce* brute_force,
    uint32_t index,
    uint32_t* record_count) {
    bool success = false;
    *record_count = 0;

    InfraredBruteForceRecordDict_it_t it;
    for(InfraredBruteForceRecordDict_it(it, brute_force->records);
        !InfraredBruteForceRecordDict_end_p(it);
        InfraredBruteForceRecordDict_next(it)) {
        const InfraredBruteForceRecordDict_itref_t* record = InfraredBruteForceRecordDict_cref(it);
        if(record->value.index == index) {
            *record_count = record->value.count;
            if(*record_count) {
                furi_string_set(brute_force->current_record_name, record->key);
            }
            break;
        }
    }

    if(*record_count) {
        Storage* storage = furi_record_open(RECORD_STORAGE);
        brute_force->ff = flipper_format_buffered_file_alloc(storage);
        success =
            flipper_format_buffered_file_open_existing(brute_force->ff, brute_force->db_filename);
        if(!success) {
            flipper_format_free(brute_force->ff);
            brute_force->ff = NULL;
            furi_record_close(RECORD_STORAGE);
        }
    }
    return success;
}

bool infrared_brute_force_is_started(InfraredBruteForce* brute_force) {
    return brute_force->ff;
}

void infrared_brute_force_stop(InfraredBruteForce* brute_force) {
    furi_assert(furi_string_size(brute_force->current_record_name));
    furi_assert(brute_force->ff);

    furi_string_reset(brute_force->current_record_name);
    flipper_format_free(brute_force->ff);
    furi_record_close(RECORD_STORAGE);
    brute_force->ff = NULL;
}

bool infrared_brute_force_send_next(InfraredBruteForce* brute_force) {
    furi_assert(furi_string_size(brute_force->current_record_name));
    furi_assert(brute_force->ff);
    bool success = false;

    FuriString* signal_name;
    signal_name = furi_string_alloc();
    InfraredSignal* signal = infrared_signal_alloc();

    do {
        success = infrared_signal_read(signal, brute_force->ff, signal_name);
    } while(success && !string_equal_p(brute_force->current_record_name, signal_name));

    if(success) {
        infrared_signal_transmit(signal);
    }

    infrared_signal_free(signal);
    furi_string_free(signal_name);
    return success;
}

void infrared_brute_force_add_record(
    InfraredBruteForce* brute_force,
    uint32_t index,
    const char* name) {
    InfraredBruteForceRecord value = {.index = index, .count = 0};
    FuriString* key;
    key = furi_string_alloc_set(name);
    InfraredBruteForceRecordDict_set_at(brute_force->records, key, value);
    furi_string_free(key);
}

void infrared_brute_force_reset(InfraredBruteForce* brute_force) {
    InfraredBruteForceRecordDict_reset(brute_force->records);
}
