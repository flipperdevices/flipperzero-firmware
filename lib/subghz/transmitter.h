#pragma once

#include "types.h"
#include "environment.h"
#include "protocols/base.h"

typedef struct SubGhzTransmitter SubGhzTransmitter;

struct SubGhzTransmitter {
    const SubGhzProtocol* protocol;
    SubGhzProtocolEncoderBase* protocol_instance;
};

SubGhzTransmitter*
    subghz_transmitter_alloc_init(SubGhzEnvironment* environment, const char* protocol_name);

void subghz_transmitter_free(SubGhzTransmitter* instance);

bool subghz_transmitter_stop(SubGhzTransmitter* instance);

bool subghz_transmitter_load(
    SubGhzTransmitter* instance,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);

LevelDuration subghz_transmitter_yield(void* context);
