#pragma once

#include "eth_device.h"

typedef struct EthWorker EthWorker;

typedef enum {
    EthWorkerStateNotInited = 0,
    EthWorkerStateModuleInit,
    EthWorkerStateModulePowerOn,
    EthWorkerStateModuleConnect,
    EthWorkerStateMACInit,
    EthWorkerStateStaticIp,
    EthWorkerStateDHCP,
    EthWorkerStatePing,
    EthWorkerStateStop,
} EthWorkerState;

typedef enum {
    EthWorkerSubstateInProcess = 0,
    EthWorkerSubStateSuccess,
    EthWorkerSubStateError,
} EthWorkerSubState;

typedef enum {
    EthCustomEventUpdate = 0,
    EthCustomEventModuleInit,
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

void eth_worker_free(EthWorker* eth_worker);

void eth_worker_start(
    EthWorker* eth_worker,
    EthWorkerState state,
    EthWorkerCallback callback,
    void* context);

void eth_worker_stop(EthWorker* eth_worker);