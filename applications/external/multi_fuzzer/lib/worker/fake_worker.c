#include "fake_worker.h"
#include "helpers/hardware_worker.h"
#include "protocol_i.h"

#include <timer.h>

#include <lib/toolbox/hex.h>
#include <toolbox/stream/stream.h>
#include <toolbox/stream/buffered_file_stream.h>

#define TAG "Fuzzer worker"
#define TOTAL_PROTOCOL_COUNT fuzzer_proto_get_count_of_protocols()
#define PROTOCOL_KEY_FOLDER EXT_PATH(PROTOCOL_KEY_FOLDER_NAME)

typedef uint8_t FuzzerWorkerPayload[MAX_PAYLOAD_SIZE];

struct FuzzerWorker {
    HardwareWorker* hw_worker;

    const FuzzerProtocol* protocol;
    HwProtocolID* suported_proto;

    FuzzerWorkerPayload payload;

    FuzzerWorkerAttackType attack_type;
    uint16_t index;
    Stream* uids_stream;

    bool in_emu_phase;
    FuriTimer* timer;
    uint16_t timer_idle_time_ms;
    uint16_t timer_emu_time_ms;

    FuzzerWorkerUidChagedCallback tick_callback;
    void* tick_context;

    FuzzerWorkerEndCallback end_callback;
    void* end_context;
};

static bool fuzzer_worker_set_protocol(FuzzerWorker* instance, FuzzerProtocolsID protocol_index) {
    if(!(protocol_index < TOTAL_PROTOCOL_COUNT)) {
        return false;
    }

    instance->protocol = &fuzzer_proto_items[protocol_index];
    return hardware_worker_set_protocol_id_by_name(
        instance->hw_worker, fuzzer_proto_items[protocol_index].name);
}

static FuzzerProtocolsID
    fuzzer_worker_is_protocol_valid(FuzzerWorker* instance, HwProtocolID protocol_id) {
    for(FuzzerProtocolsID i = 0; i < TOTAL_PROTOCOL_COUNT; i++) {
        if(protocol_id == instance->suported_proto[i]) {
            return i;
        }
    }
    return TOTAL_PROTOCOL_COUNT;
}

FuzzerWorkerLoadKeyState fuzzer_worker_load_key_from_file(
    FuzzerWorker* instance,
    FuzzerProtocolsID* protocol_index,
    const char* filename) {
    furi_assert(instance);

    FuzzerWorkerLoadKeyState res = FuzzerWorkerLoadKeyStateUnsuportedProto;
    if(!hardware_worker_load_key_from_file(instance->hw_worker, filename)) {
        FURI_LOG_E(TAG, "Load key file: cant load file");
        res = FuzzerWorkerLoadKeyStateBadFile;
    } else {
        FuzzerProtocolsID loaded_id = fuzzer_worker_is_protocol_valid(
            instance, hardware_worker_get_protocol_id(instance->hw_worker));

        if(!fuzzer_worker_set_protocol(instance, loaded_id)) {
            FURI_LOG_E(TAG, "Load key file: Unsuported protocol");
            res = FuzzerWorkerLoadKeyStateUnsuportedProto;
        } else {
            if(*protocol_index != loaded_id) {
                res = FuzzerWorkerLoadKeyStateDifferentProto;
            } else {
                res = FuzzerWorkerLoadKeyStateOk;
            }
            *protocol_index = loaded_id;

            hardware_worker_get_protocol_data(
                instance->hw_worker, &instance->payload[0], MAX_PAYLOAD_SIZE);
        }
    }

    return res;
}

static bool fuzer_worker_make_key_folder() {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    const bool res = storage_simply_mkdir(storage, PROTOCOL_KEY_FOLDER);

    furi_record_close(RECORD_STORAGE);

    return res;
}

bool fuzzer_worker_save_key(FuzzerWorker* instance, const char* path) {
    furi_assert(instance);
    bool res = false;

    if(!fuzer_worker_make_key_folder()) {
        FURI_LOG_E(TAG, "Cannot create key folder");
    } else if(!hardware_worker_save_key(instance->hw_worker, path)) {
        FURI_LOG_E(TAG, "Cannot save key file");
    } else {
        FURI_LOG_D(TAG, "Save key Success");
        res = true;
    }

    return res;
}

static bool fuzzer_worker_load_key(FuzzerWorker* instance, bool next) {
    furi_assert(instance);
    furi_assert(instance->protocol);
    bool res = false;

    const FuzzerProtocol* protocol = instance->protocol;

    switch(instance->attack_type) {
    case FuzzerWorkerAttackTypeDefaultDict:
        if(next) {
            if(instance->index < (protocol->dict.len - 1)) {
                instance->index++;
            } else {
                break;
            }
        }
        if(instance->index < protocol->dict.len) {
            memcpy(
                instance->payload,
                &protocol->dict.val[instance->index * protocol->data_size],
                protocol->data_size);
            res = true;
        }
        break;

    case FuzzerWorkerAttackTypeLoadFileCustomUids: {
        if(next) {
            instance->index++;
        }
        uint8_t str_len = protocol->data_size * 2 + 1;
        FuriString* data_str = furi_string_alloc();
        while(true) {
            furi_string_reset(data_str);
            if(!stream_read_line(instance->uids_stream, data_str)) {
                stream_rewind(instance->uids_stream);
                // TODO Check empty file & close stream and storage
                break;
            } else if(furi_string_get_char(data_str, 0) == '#') {
                // Skip comment string
                continue;
            } else if(furi_string_size(data_str) != str_len) {
                // Ignore strin with bad length
                FURI_LOG_W(TAG, "Bad string length");
                continue;
            } else {
                FURI_LOG_D(TAG, "Uid candidate: \"%s\"", furi_string_get_cstr(data_str));
                bool parse_ok = true;
                for(uint8_t i = 0; i < protocol->data_size; i++) {
                    if(!hex_char_to_uint8(
                           furi_string_get_cstr(data_str)[i * 2],
                           furi_string_get_cstr(data_str)[i * 2 + 1],
                           &instance->payload[i])) {
                        parse_ok = false;
                        break;
                    }
                }
                res = parse_ok;
            }
            break;
        }
        furi_string_free(data_str);
    }

    break;

    case FuzzerWorkerAttackTypeLoadFile:
        if(instance->payload[instance->index] != 0xFF) {
            instance->payload[instance->index]++;
            res = true;
        }

        break;

    default:
        break;
    }

    if(res) {
        hardware_worker_set_protocol_data(
            instance->hw_worker, &instance->payload[0], protocol->data_size);
    }

    return res;
}

static bool fuzzer_worker_load_previous_key(FuzzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol);
    bool res = false;

    const FuzzerProtocol* protocol = instance->protocol;

    switch(instance->attack_type) {
    case FuzzerWorkerAttackTypeDefaultDict:
        if(instance->index > 0) {
            instance->index--;
            memcpy(
                instance->payload,
                &protocol->dict.val[instance->index * protocol->data_size],
                protocol->data_size);
            res = true;
        }
        break;

    case FuzzerWorkerAttackTypeLoadFile:
        if(instance->payload[instance->index] != 0x00) {
            instance->payload[instance->index]--;
            res = true;
        }

        break;

    default:
        break;
    }

    if(res) {
        hardware_worker_set_protocol_data(
            instance->hw_worker, &instance->payload[0], protocol->data_size);
    }

    return res;
}

static void fuzzer_worker_on_tick_callback(void* context) {
    furi_assert(context);

    FuzzerWorker* instance = context;

    if(instance->in_emu_phase) {
        hardware_worker_stop(instance->hw_worker);
        instance->in_emu_phase = false;
        furi_timer_start(instance->timer, furi_ms_to_ticks(instance->timer_idle_time_ms));
    } else {
        if(!fuzzer_worker_load_key(instance, true)) {
            fuzzer_worker_pause(instance); // XXX
            if(instance->end_callback) {
                instance->end_callback(instance->end_context);
            }
        } else {
            hardware_worker_emulate_start(instance->hw_worker);
            instance->in_emu_phase = true;
            furi_timer_start(instance->timer, furi_ms_to_ticks(instance->timer_emu_time_ms));
            if(instance->tick_callback) {
                instance->tick_callback(instance->tick_context);
            }
        }
    }
}

void fuzzer_worker_get_current_key(FuzzerWorker* instance, FuzzerPayload* output_key) {
    furi_assert(instance);
    furi_assert(output_key);
    furi_assert(instance->protocol);

    output_key->data_size = instance->protocol->data_size;
    memcpy(output_key->data, instance->payload, instance->protocol->data_size);
}

bool fuzzer_worker_next_key(FuzzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol);

    return fuzzer_worker_load_key(instance, true);
}

bool fuzzer_worker_previous_key(FuzzerWorker* instance) {
    furi_assert(instance);
    furi_assert(instance->protocol);

    return fuzzer_worker_load_previous_key(instance);
}

bool fuzzer_worker_init_attack_dict(FuzzerWorker* instance, FuzzerProtocolsID protocol_index) {
    furi_assert(instance);

    bool res = false;

    if(!fuzzer_worker_set_protocol(instance, protocol_index)) {
        instance->attack_type = FuzzerWorkerAttackTypeMax;
        return res;
    }

    instance->attack_type = FuzzerWorkerAttackTypeDefaultDict;
    instance->index = 0;

    if(!fuzzer_worker_load_key(instance, false)) {
        instance->attack_type = FuzzerWorkerAttackTypeMax;
    } else {
        res = true;
    }

    return res;
}

bool fuzzer_worker_init_attack_file_dict(
    FuzzerWorker* instance,
    FuzzerProtocolsID protocol_index,
    FuriString* file_path) {
    furi_assert(instance);
    furi_assert(file_path);

    bool res = false;

    if(!fuzzer_worker_set_protocol(instance, protocol_index)) {
        instance->attack_type = FuzzerWorkerAttackTypeMax;
        return res;
    }

    Storage* storage = furi_record_open(RECORD_STORAGE);
    instance->uids_stream = buffered_file_stream_alloc(storage);
    furi_record_close(RECORD_STORAGE);

    if(!buffered_file_stream_open(
           instance->uids_stream, furi_string_get_cstr(file_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
        buffered_file_stream_close(instance->uids_stream);
        return res;
    }

    instance->attack_type = FuzzerWorkerAttackTypeLoadFileCustomUids;
    instance->index = 0;

    if(!fuzzer_worker_load_key(instance, false)) {
        instance->attack_type = FuzzerWorkerAttackTypeMax;
        buffered_file_stream_close(instance->uids_stream);
        stream_free(instance->uids_stream);
    } else {
        res = true;
    }

    return res;
}

bool fuzzer_worker_init_attack_bf_byte(
    FuzzerWorker* instance,
    FuzzerProtocolsID protocol_index,
    const FuzzerPayload* new_uid,
    uint8_t chusen) {
    furi_assert(instance);

    bool res = false;
    if(!fuzzer_worker_set_protocol(instance, protocol_index)) {
        instance->attack_type = FuzzerWorkerAttackTypeMax;
        return res;
    }

    instance->attack_type = FuzzerWorkerAttackTypeLoadFile;
    instance->index = chusen;

    memcpy(instance->payload, new_uid->data, instance->protocol->data_size);

    hardware_worker_set_protocol_data(
        instance->hw_worker, &instance->payload[0], instance->protocol->data_size);

    return true;
}

FuzzerWorker* fuzzer_worker_alloc() {
    FuzzerWorker* instance = malloc(sizeof(FuzzerWorker));

    instance->hw_worker = hardware_worker_alloc();
    hardware_worker_start_thread(instance->hw_worker);

    instance->suported_proto = malloc(sizeof(HwProtocolID) * TOTAL_PROTOCOL_COUNT);

    for(uint8_t i = 0; i < TOTAL_PROTOCOL_COUNT; i++) {
        if(!hardware_worker_set_protocol_id_by_name(
               instance->hw_worker, fuzzer_proto_items[i].name)) {
            // Check protocol support
            FURI_LOG_E(TAG, "Not supported protocol name: %s", fuzzer_proto_items[i].name);
            furi_crash("Not supported protocol name");
        } else {
            instance->suported_proto[i] = hardware_worker_get_protocol_id(instance->hw_worker);
            FURI_LOG_D(
                TAG,
                "%u: %15s Protocol_id: %lu",
                i + 1,
                fuzzer_proto_items[i].name,
                instance->suported_proto[i]);
        }
    }

    instance->attack_type = FuzzerWorkerAttackTypeMax;
    instance->index = 0;
    instance->in_emu_phase = false;

    memset(instance->payload, 0x00, sizeof(instance->payload));

    instance->timer_idle_time_ms = PROTOCOL_DEF_IDLE_TIME * 100;
    instance->timer_emu_time_ms = PROTOCOL_DEF_EMU_TIME * 100;

    instance->timer =
        furi_timer_alloc(fuzzer_worker_on_tick_callback, FuriTimerTypeOnce, instance);

    return instance;
}

void fuzzer_worker_free(FuzzerWorker* instance) {
    furi_assert(instance);

    fuzzer_worker_stop(instance);

    furi_timer_free(instance->timer);

    free(instance->suported_proto);

    hardware_worker_stop_thread(instance->hw_worker);
    hardware_worker_free(instance->hw_worker);

    free(instance);
}

bool fuzzer_worker_start(FuzzerWorker* instance, uint8_t idle_time, uint8_t emu_time) {
    furi_assert(instance);

    if(instance->attack_type < FuzzerWorkerAttackTypeMax) {
        if(idle_time == 0) {
            instance->timer_idle_time_ms = 10;
        } else {
            instance->timer_idle_time_ms = idle_time * 100;
        }
        if(emu_time == 0) {
            instance->timer_emu_time_ms = 10;
        } else {
            instance->timer_emu_time_ms = emu_time * 100;
        }

        FURI_LOG_D(
            TAG,
            "Emu_time %u ms  Idle_time %u ms",
            instance->timer_emu_time_ms,
            instance->timer_idle_time_ms);

        hardware_worker_emulate_start(instance->hw_worker);

        instance->in_emu_phase = true;
        furi_timer_start(instance->timer, furi_ms_to_ticks(instance->timer_emu_time_ms));
        return true;
    }
    return false;
}

void fuzzer_worker_start_emulate(FuzzerWorker* instance) {
    furi_assert(instance);

    hardware_worker_emulate_start(instance->hw_worker);
}

void fuzzer_worker_pause(FuzzerWorker* instance) {
    furi_assert(instance);

    furi_timer_stop(instance->timer);

    hardware_worker_stop(instance->hw_worker);
}

void fuzzer_worker_stop(FuzzerWorker* instance) {
    furi_assert(instance);

    furi_timer_stop(instance->timer);

    hardware_worker_stop(instance->hw_worker);

    if(instance->attack_type == FuzzerWorkerAttackTypeLoadFileCustomUids) {
        buffered_file_stream_close(instance->uids_stream);
        stream_free(instance->uids_stream);
        instance->attack_type = FuzzerWorkerAttackTypeMax;
    }

    // TODO anything else
}

void fuzzer_worker_set_uid_chaged_callback(
    FuzzerWorker* instance,
    FuzzerWorkerUidChagedCallback callback,
    void* context) {
    furi_assert(instance);
    instance->tick_callback = callback;
    instance->tick_context = context;
}

void fuzzer_worker_set_end_callback(
    FuzzerWorker* instance,
    FuzzerWorkerEndCallback callback,
    void* context) {
    furi_assert(instance);
    instance->end_callback = callback;
    instance->end_context = context;
}
