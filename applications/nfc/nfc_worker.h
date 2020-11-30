#pragma once

#include <cmsis_os2.h>
#include <stdbool.h>
#include "nfc_i.h"

typedef enum {
    NfcWorkerCommandTypePoll,
    NfcWorkerCommandTypeExit,
} NfcWorkerCommandType;

typedef struct {
    NfcWorkerCommandType type;
} NfcWorkerCommand;


typedef enum {
    NfcWorkerResponseTypeDeviceFound,
    NfcWorkerResponseTypeDeviceError,
} NfcWorkerResponseType;

typedef struct {
    NfcWorkerResponseType type;
    union {
        NfcDevice device;
    };
} NfcWorkerResponse;


typedef struct {
    osMessageQueueId_t command_queue;
    osMessageQueueId_t response_queue;
} NfcWorkerContext;

void nfc_worker_task(NfcWorkerContext* context);
void nfc_worker_poll(NfcWorkerContext* context);

ReturnCode nfc_worker_nfca_poll(NfcWorkerContext* context);
ReturnCode nfc_worker_nfcb_poll(NfcWorkerContext* context);
ReturnCode nfc_worker_nfcf_poll(NfcWorkerContext* context);
ReturnCode nfc_worker_nfcv_poll(NfcWorkerContext* context);
