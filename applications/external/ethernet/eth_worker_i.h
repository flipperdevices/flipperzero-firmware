#pragma once

#include <furi.h>
#include "eth_worker.h"
#include "eth_view_process.h"
#include "eth_save_process.h"

struct EthWorker {
    FuriThread* thread;
    void* context;
    EthernetSaveConfig* config;
    EthViewProcess* init_process;
    EthViewProcess* dhcp_process;
    EthViewProcess* stat_process;
    EthViewProcess* ping_process;
    EthViewProcess* reset_process;
    EthViewProcess* active_process;

    EthWorkerState state;
    EthWorkerState next_state;
    EthWorkerSubState sub_state;
    EthWorkerCallback callback;
    FuriTimer* timer;
};

void eth_worker_change_state(EthWorker* eth_worker, EthWorkerState state);
void eth_worker_log(EthWorker* eth_worker, const char* str);
void eth_run(EthWorker* worker, EthWorkerProcess process);
void eth_log(EthWorkerProcess process, const char* format, ...);

int32_t eth_worker_task(void* context);
