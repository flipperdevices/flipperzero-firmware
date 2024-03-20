#pragma once

#include <lib/nfc/protocols/iso14443_4a/iso14443_4a_poller.h>
#include <lib/nfc/protocols/mf_classic/mf_classic_poller.h>

#include "sam_api.h"
#include "seader_credential.h"
#include "seader_bridge.h"

typedef struct SeaderWorker SeaderWorker;
typedef struct CCID_Message CCID_Message;
typedef struct SeaderAPDU SeaderAPDU;

typedef enum {
    // Init states
    SeaderWorkerStateNone,
    SeaderWorkerStateBroken,
    SeaderWorkerStateReady,
    // Main worker states
    SeaderWorkerStateCheckSam,
    SeaderWorkerStateVirtualCredential,
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

typedef enum {
    SeaderPollerEventTypeCardDetect,
    SeaderPollerEventTypeConversation,
    SeaderPollerEventTypeComplete,

    SeaderPollerEventTypeSuccess,
    SeaderPollerEventTypeFail,
} SeaderPollerEventType;

typedef void (*SeaderWorkerCallback)(SeaderWorkerEvent event, void* context);

SeaderWorker* seader_worker_alloc();

SeaderWorkerState seader_worker_get_state(SeaderWorker* seader_worker);

void seader_worker_free(SeaderWorker* seader_worker);

void seader_worker_start(
    SeaderWorker* seader_worker,
    SeaderWorkerState state,
    SeaderUartBridge* uart,
    SeaderWorkerCallback callback,
    void* context);

void seader_worker_stop(SeaderWorker* seader_worker);
bool seader_worker_process_sam_message(Seader* seader, CCID_Message* message);
void seader_worker_send_version(Seader* seader);

NfcCommand seader_worker_poller_callback_iso14443_4a(NfcGenericEvent event, void* context);
NfcCommand seader_worker_poller_callback_mfc(NfcGenericEvent event, void* context);
NfcCommand seader_worker_poller_callback_picopass(PicopassPollerEvent event, void* context);
