#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <furi/core/mutex.h>

typedef uint8_t TotpBtTypeCodeWorkerEvent;

typedef struct TotpBtTypeCodeWorkerContext TotpBtTypeCodeWorkerContext;

enum TotpBtTypeCodeWorkerEvents {
    TotpBtTypeCodeWorkerEventReserved = 0b00,
    TotpBtTypeCodeWorkerEventStop = 0b01,
    TotpBtTypeCodeWorkerEventType = 0b10
};

TotpBtTypeCodeWorkerContext* totp_bt_type_code_worker_init();
void totp_bt_type_code_worker_free(TotpBtTypeCodeWorkerContext* context);
void totp_bt_type_code_worker_start(
    TotpBtTypeCodeWorkerContext* context,
    char* code_buffer,
    uint8_t code_buffer_size,
    FuriMutex* code_buffer_sync);
void totp_bt_type_code_worker_stop(TotpBtTypeCodeWorkerContext* context);
void totp_bt_type_code_worker_notify(
    TotpBtTypeCodeWorkerContext* context,
    TotpBtTypeCodeWorkerEvent event,
    uint8_t flags);

bool totp_bt_type_code_worker_is_advertising(const TotpBtTypeCodeWorkerContext* context);