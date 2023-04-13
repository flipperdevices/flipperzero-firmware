#pragma once

#include <stdbool.h>

typedef struct TuLlaveWorker TuLlaveWorker;

typedef enum {
    TuLlaveWorkerStateReady,
    TuLlaveWorkerStateCheck,
    TuLlaveWorkerStateStop,
} TuLlaveWorkerState;

typedef enum {
    TuLlaveWorkerEventSuccess,
    TuLlaveWorkerEventFail,
    TuLlaveWorkerEventCardDetected,
    TuLLaveWorkerEventNoCardDetected,
} TuLlaveWorkerEvent;

typedef bool (*TuLlaveWorkerCallback)(TuLlaveWorkerEvent event, void* context);

TuLlaveWorker* tullave_worker_alloc();

void tullave_worker_free(TuLlaveWorker* t_worker);

void tullave_worker_stop(TuLlaveWorker* t_worker);

void tullave_worker_start(TuLlaveWorker* t_worker, TuLlaveWorkerCallback callback, void* context);