#include "hardware_worker.h"
#include "furi.h"

#if defined(RFID_125_PROTOCOL)

#include <lib/lfrfid/lfrfid_dict_file.h>
#include <lib/lfrfid/lfrfid_worker.h>

#else

#include <lib/ibutton/ibutton_worker.h>
#include <lib/ibutton/ibutton_key.h>

#endif

#define TAG "Fuzzer HW worker"

struct HardwareWorker {
#if defined(RFID_125_PROTOCOL)
    LFRFIDWorker* proto_worker;
    ProtocolId protocol_id;
    ProtocolDict* protocols_items;
#else
    iButtonWorker* proto_worker;
    iButtonProtocolId protocol_id;
    iButtonProtocols* protocols_items;
    iButtonKey* key;
#endif
};

HardwareWorker* hardware_worker_alloc() {
    HardwareWorker* instance = malloc(sizeof(HardwareWorker));
#if defined(RFID_125_PROTOCOL)
    instance->protocols_items = protocol_dict_alloc(lfrfid_protocols, LFRFIDProtocolMax);

    instance->proto_worker = lfrfid_worker_alloc(instance->protocols_items);
#else
    instance->protocols_items = ibutton_protocols_alloc();
    instance->key =
        ibutton_key_alloc(ibutton_protocols_get_max_data_size(instance->protocols_items));

    instance->proto_worker = ibutton_worker_alloc(instance->protocols_items);
#endif
    return instance;
}

void hardware_worker_free(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    lfrfid_worker_free(instance->proto_worker);

    protocol_dict_free(instance->protocols_items);
#else
    ibutton_worker_free(instance->proto_worker);

    ibutton_key_free(instance->key);
    ibutton_protocols_free(instance->protocols_items);
#endif
    free(instance);
}

void hardware_worker_start_thread(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    lfrfid_worker_start_thread(instance->proto_worker);
#else
    ibutton_worker_start_thread(instance->proto_worker);
#endif
}

void hardware_worker_stop_thread(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    lfrfid_worker_stop(instance->proto_worker);
    lfrfid_worker_stop_thread(instance->proto_worker);
#else
    ibutton_worker_stop(instance->proto_worker);
    ibutton_worker_stop_thread(instance->proto_worker);
#endif
}

void hardware_worker_emulate_start(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    lfrfid_worker_emulate_start(instance->proto_worker, instance->protocol_id);
#else
    ibutton_worker_emulate_start(instance->proto_worker, instance->key);
#endif
}

void hardware_worker_stop(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    lfrfid_worker_stop(instance->proto_worker);
#else
    ibutton_worker_stop(instance->proto_worker);
#endif
}

void hardware_worker_set_protocol_data(
    HardwareWorker* instance,
    uint8_t* payload,
    uint8_t payload_size) {
#if defined(RFID_125_PROTOCOL)
    protocol_dict_set_data(
        instance->protocols_items, instance->protocol_id, payload, payload_size);
#else
    ibutton_key_set_protocol_id(instance->key, instance->protocol_id);
    iButtonEditableData data;
    ibutton_protocols_get_editable_data(instance->protocols_items, instance->key, &data);

    furi_check(payload_size >= data.size);
    memcpy(data.ptr, payload, data.size);
#endif
}

void hardware_worker_get_protocol_data(
    HardwareWorker* instance,
    uint8_t* payload,
    uint8_t payload_size) {
#if defined(RFID_125_PROTOCOL)
    protocol_dict_get_data(
        instance->protocols_items, instance->protocol_id, payload, payload_size);
#else
    iButtonEditableData data;
    ibutton_protocols_get_editable_data(instance->protocols_items, instance->key, &data);
    furi_check(payload_size >= data.size);
    memcpy(payload, data.ptr, data.size);
#endif
}

static bool hardware_worker_protocol_is_valid(HardwareWorker* instance) {
#if defined(RFID_125_PROTOCOL)
    if(instance->protocol_id != PROTOCOL_NO) {
        return true;
    }
#else
    if(instance->protocol_id != iButtonProtocolIdInvalid) {
        return true;
    }
#endif
    return false;
}

bool hardware_worker_set_protocol_id_by_name(HardwareWorker* instance, const char* protocol_name) {
#if defined(RFID_125_PROTOCOL)
    instance->protocol_id =
        protocol_dict_get_protocol_by_name(instance->protocols_items, protocol_name);
    return (instance->protocol_id != PROTOCOL_NO);
#else
    instance->protocol_id =
        ibutton_protocols_get_id_by_name(instance->protocols_items, protocol_name);
    return (instance->protocol_id != iButtonProtocolIdInvalid);
#endif
}

HwProtocolID hardware_worker_get_protocol_id(HardwareWorker* instance) {
    if(hardware_worker_protocol_is_valid(instance)) {
        return instance->protocol_id;
    }
    return -1;
}

bool hardware_worker_load_key_from_file(HardwareWorker* instance, const char* filename) {
    bool res = false;

#if defined(RFID_125_PROTOCOL)
    ProtocolId loaded_proto_id = lfrfid_dict_file_load(instance->protocols_items, filename);
    if(loaded_proto_id == PROTOCOL_NO) {
        // Err Cant load file
        FURI_LOG_W(TAG, "Cant load file");
    } else {
        instance->protocol_id = loaded_proto_id;
        res = true;
    }
#else

    if(!ibutton_protocols_load(instance->protocols_items, instance->key, filename)) {
        // Err Cant load file
        FURI_LOG_W(TAG, "Cant load file");
    } else {
        instance->protocol_id = ibutton_key_get_protocol_id(instance->key);
        res = true;
    }

#endif

    return res;
}

bool hardware_worker_save_key(HardwareWorker* instance, const char* path) {
    furi_assert(instance);
    bool res;

#if defined(RFID_125_PROTOCOL)
    res = lfrfid_dict_file_save(instance->protocols_items, instance->protocol_id, path);
#else

    res = ibutton_protocols_save(instance->protocols_items, instance->key, path);
#endif

    return res;
}