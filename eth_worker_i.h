#pragma once

#include <furi.h>
#include "eth_worker.h"
#include "eth_view_process.h"
#include "eth_save_process.h"

struct EthWorkerNetConf {
    uint8_t mac[6];
    uint8_t ip[4];
    uint8_t mask[4];
    uint8_t gateway[4];
    uint8_t dns[4];
    uint8_t fifo_sizes[16];
    uint8_t is_dhcp;
};

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
    EthWorkerSubState sub_state;
    EthWorkerCallback callback;
};

void eth_worker_change_state(EthWorker* eth_worker, EthWorkerState state);
void eth_worker_log(EthWorker* eth_worker, const char* str);
void eth_log(EthWorkerProcess process, const char* format, ...);

int32_t eth_worker_task(void* context);
