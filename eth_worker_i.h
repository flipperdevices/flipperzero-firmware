#pragma once

#include "eth_worker.h"

struct EthWorker {
    FuriThread* thread;
    void* context;

    EthWorkerState state;
    EthWorkerSubState sub_state;
    char state_string[64];

    EthWorkerCallback callback;
};

void eth_worker_change_state(EthWorker* eth_worker, EthWorkerState state);

int32_t eth_worker_task(void* context);
