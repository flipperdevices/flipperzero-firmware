#pragma once

#include <gui/view.h>
#include <lib/subghz/protocols/subghz_protocol_common.h>
#include <lib/subghz/protocols/subghz_protocol.h>
#include <lib/subghz/subghz_worker.h>
#include "../subghz_history.h"



typedef enum {
    SubghzReceverEventSave,
    SubghzReceverEventBack,
} SubghzReceverEvent;

typedef struct SubghzReceiver SubghzReceiver;

typedef void (*SubghzReceiverCallback)(SubghzReceverEvent event, void* context);

void subghz_receiver_set_callback(
    SubghzReceiver* subghz_receiver,
    SubghzReceiverCallback callback,
    void* context);

SubghzReceiver* subghz_receiver_alloc();

void subghz_receiver_free(SubghzReceiver* subghz_receiver);

View* subghz_receiver_get_view(SubghzReceiver* subghz_receiver);

void subghz_receiver_set_protocol(
    SubghzReceiver* subghz_receiver,
    SubGhzProtocolCommon* protocol_result,
    SubGhzProtocol* protocol);
void subghz_receiver_set_worker(SubghzReceiver* subghz_receiver, SubGhzWorker* worker);
void subghz_receiver_set_history(SubghzReceiver* subghz_receiver, SubGhzHistory* history);