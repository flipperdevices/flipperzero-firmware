#pragma once

#include <stdint.h>

typedef struct EthWorker EthWorker;
typedef struct EthViewProcess EthViewProcess;

typedef enum {
    EthWorkerStateNotAllocated = 0,
    EthWorkerStateNotInited,
    EthWorkerStateDefaultNext,
    EthWorkerStateInited,
    EthWorkerStateInit,
    EthWorkerStateModulePowerOn,
    EthWorkerStateModuleConnect,
    EthWorkerStateMACInit,
    EthWorkerStateStaticIp,
    EthWorkerStateDHCP,
    EthWorkerStateOnline,
    EthWorkerStatePing,
    EthWorkerStateStop,
    EthWorkerStateReset,
} EthWorkerState;

typedef enum {
    EthWorkerProcessInit,
    EthWorkerProcessDHCP,
    EthWorkerProcessStatic,
    EthWorkerProcessPing,
    EthWorkerProcessReset,
    EthWorkerProcessActive,
    EthWorkerProcessExit,
} EthWorkerProcess;

typedef enum {
    EthWorkerSubstateInProcess = 0,
    EthWorkerSubStateSuccess,
    EthWorkerSubStateError,
} EthWorkerSubState;

typedef enum {
    EthCustomEventUpdate = 0,
    EthCustomEventModuleInit,
    EthCustomEventModuleError,
    EthCustomEventModulePowerOn,
    EthCustomEventModuleConnect,
    EthCustomEventMACInit,
    EthCustomEventStaticIp,
    EthCustomEventDHCP,
    EthCustomEventPing,
    EthCustomEventTCP2UART,
    EthCustomEventTCP2CLI,
    EthCustomEventSaved,
} EthCustomEvent;

typedef void (*EthWorkerCallback)(EthCustomEvent event, void* context);

EthWorker* eth_worker_alloc();

EthWorkerState eth_worker_get_state(EthWorker* eth_worker);
void eth_worker_set_active_process(EthWorker* eth_worker, EthWorkerProcess state);

void eth_worker_free(EthWorker* eth_worker);

void eth_worker_start(
    EthWorker* eth_worker,
    EthWorkerState state,
    EthWorkerCallback callback,
    void* context);

void eth_worker_stop(EthWorker* eth_worker);
void eth_worker_dhcp(EthWorker* eth_worker);
void eth_worker_w5500(EthWorker* eth_worker);
void eth_worker_init_process(EthWorker* eth_worker);

#define PING_SOCKET 1
uint8_t ping_auto_interface(uint8_t* adress);
void dhcp_timer_callback(void* context);
