#pragma once

#include <furi.h>
#include "eth_worker.h"
#include "eth_view_process.h"

struct EthWorkerNetConf {
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    uint8_t fifo_sizes[16];
    uint8_t is_dhcp;
};

struct EthViewProcess {
    char fifo[SCREEN_STRINGS_COUNT][SCREEN_SYMBOLS_WIDTH];
    uint8_t x;
    uint8_t y;
    uint8_t carriage;
    uint8_t position;
    uint8_t autofill;
};

struct EthWorker {
    FuriThread* thread;
    void* context;
    EthViewProcess* init_process;

    EthWorkerState state;
    EthWorkerSubState sub_state;
    EthWorkerCallback callback;
};

void eth_worker_change_state(EthWorker* eth_worker, EthWorkerState state);

int32_t eth_worker_task(void* context);
