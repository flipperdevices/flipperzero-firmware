#pragma once

#include "seader_credential.h"
#include "sub.h"

typedef struct SeaderWorker SeaderWorker;
typedef struct CCID_Message CCID_Message;

typedef enum {
    // Init states
    SeaderWorkerStateNone,
    SeaderWorkerStateBroken,
    SeaderWorkerStateReady,
    // Main worker states
    SeaderWorkerStateCheckSam,
    SeaderWorkerStateReadPicopass,
    SeaderWorkerStateRead14a,
    // Transition
    SeaderWorkerStateStop,
} SeaderWorkerState;

typedef enum {
    // Reserve first 50 events for application events
    SeaderWorkerEventReserved = 50,

    // Seader worker common events
    SeaderWorkerEventSuccess,
    SeaderWorkerEventFail,
    SeaderWorkerEventSamPresent,
    SeaderWorkerEventSamWrong,
    SeaderWorkerEventSamMissing,
    SeaderWorkerEventNoCardDetected,
    SeaderWorkerEventStartReading,
} SeaderWorkerEvent;

typedef void (*SeaderWorkerCallback)(SeaderWorkerEvent event, void* context);

SeaderWorker* seader_worker_alloc();

SeaderWorkerState seader_worker_get_state(SeaderWorker* seader_worker);

void seader_worker_free(SeaderWorker* seader_worker);

void seader_worker_start(
    SeaderWorker* seader_worker,
    SeaderWorkerState state,
    SeaderUartBridge* uart,
    SeaderCredential* credential,
    SeaderWorkerCallback callback,
    void* context);

void seader_worker_stop(SeaderWorker* seader_worker);
void seader_worker_process_message(SeaderWorker* seader_worker, CCID_Message* message);
