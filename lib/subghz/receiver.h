#pragma once

#include "types.h"
#include "protocols/base.h"

typedef struct SubGhzReceiver SubGhzReceiver;

typedef void (*SubGhzReceiverCallback)(
    SubGhzReceiver* decoder,
    SubGhzProtocolDecoderBase* decoder_base,
    void* context);

SubGhzReceiver* subghz_receiver_alloc(void);

void subghz_receiver_free(SubGhzReceiver* instance);

void subghz_receiver_decode(SubGhzReceiver* instance, bool level, uint32_t duration);

void subghz_receiver_reset(SubGhzReceiver* instance);

void subghz_receiver_set_rx_callback(
    SubGhzReceiver* instance,
    SubGhzReceiverCallback callback,
    void* context);
