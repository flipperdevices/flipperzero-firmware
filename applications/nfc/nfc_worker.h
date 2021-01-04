#pragma once

typedef struct NfcWorker NfcWorker;

NfcWorker* nfc_worker_alloc(osMessageQueueId_t message_queue);

void nfc_worker_free(NfcWorker* nfc_worker);

void nfc_worker_poll_start(NfcWorker* nfc_worker);

void nfc_worker_poll_stop(NfcWorker* nfc_worker);

void nfc_worker_field_on(NfcWorker* nfc_worker);

void nfc_worker_field_off(NfcWorker* nfc_worker);
