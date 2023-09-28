#pragma once

#include <furi.h>
#include <furi_hal.h>

#include <lib/subghz/receiver.h>
#include <lib/subghz/protocols/protocol_items.h>
#include <lib/subghz/devices/cc1101_int/cc1101_int_interconnect.h>
#include <lib/subghz/devices/devices.h>

typedef void (*SubghzPacketCallback)(FuriString* buffer, void* context);

typedef enum {
    SUBGHZ_RECEIVER_INITIALIZING,
    SUBGHZ_RECEIVER_LISTENING,
    SUBGHZ_RECEIVER_NOTLISTENING,
    SUBGHZ_RECEIVER_UNINITIALING,
    SUBGHZ_RECEIVER_UNINITIALIZED,
} SubghzReceiverState;

typedef struct {
    SubGhzEnvironment* environment;
    FuriStreamBuffer* stream;
    FuriThread* thread;
    SubGhzReceiver* receiver;
    bool overrun;
    SubghzReceiverState status;
    SubghzPacketCallback callback;
    void* callback_context;
} GenieSubGhz;

GenieSubGhz* genie_subghz_alloc();
void genie_subghz_free(GenieSubGhz* subghz);

void start_listening(
    GenieSubGhz* context,
    uint32_t frequency,
    SubghzPacketCallback callback,
    void* callback_context);
void stop_listening(GenieSubGhz* context);