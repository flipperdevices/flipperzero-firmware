#include "transmitter.h"

#include "protocols/base.h"
#include "protocols/registry.h"

//encoder func
SubGhzTransmitter* subghz_transmitter_alloc_init(const char* protocol_name) {
    SubGhzTransmitter* instance = NULL;
    const SubGhzProtocol* protocol = subghz_protocol_registry_get_by_name(protocol_name);

    if(protocol && protocol->encoder && protocol->encoder->alloc) {
        instance = furi_alloc(sizeof(SubGhzTransmitter));
        instance->protocol = protocol;
        instance->protocol_instance = instance->protocol->encoder->alloc();
    }

    return instance;
}

void subghz_transmitter_free(SubGhzTransmitter* instance) {
    furi_assert(instance);
    instance->protocol->encoder->free(instance->protocol_instance);
    free(instance);
}

void subghz_transmitter_stop(SubGhzTransmitter* instance) {
    furi_assert(instance);
    instance->protocol->encoder->stop(instance->protocol_instance);
}

void subghz_transmitter_load(
    SubGhzTransmitter* instance,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat) {
    furi_assert(instance);

    instance->protocol->encoder->load(instance->protocol_instance, key, count_bit, repeat);
}

LevelDuration subghz_transmitter_yield(void* context) {
    SubGhzTransmitter* instance = context;

    return instance->protocol->encoder->yield(instance->protocol_instance);
}
