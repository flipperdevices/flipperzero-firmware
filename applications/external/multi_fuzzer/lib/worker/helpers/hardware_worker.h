#pragma once

#include <stdint.h>
#include <stdbool.h>

#if defined(RFID_125_PROTOCOL)

#include <lib/lfrfid/lfrfid_dict_file.h>
typedef ProtocolId HwProtocolID;

#else

#include <lib/ibutton/protocols/protocol_common.h>
typedef iButtonProtocolId HwProtocolID;

#endif

typedef struct HardwareWorker HardwareWorker;

HardwareWorker* hardware_worker_alloc();

void hardware_worker_free(HardwareWorker* instance);

void hardware_worker_start_thread(HardwareWorker* instance);

void hardware_worker_stop_thread(HardwareWorker* instance);

void hardware_worker_emulate_start(HardwareWorker* instance);

void hardware_worker_stop(HardwareWorker* instance);

void hardware_worker_set_protocol_data(
    HardwareWorker* instance,
    uint8_t* payload,
    uint8_t payload_size);

void hardware_worker_get_protocol_data(
    HardwareWorker* instance,
    uint8_t* payload,
    uint8_t payload_size);

bool hardware_worker_set_protocol_id_by_name(HardwareWorker* instance, const char* protocol_name);

HwProtocolID hardware_worker_get_protocol_id(HardwareWorker* instance);

bool hardware_worker_load_key_from_file(HardwareWorker* instance, const char* filename);

bool hardware_worker_save_key(HardwareWorker* instance, const char* path);