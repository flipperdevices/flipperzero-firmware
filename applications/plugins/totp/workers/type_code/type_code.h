#pragma once

#include <stdlib.h>
#include <furi/furi.h>
#include <furi_hal.h>

typedef struct {
    char* string;
    uint8_t string_length;
    FuriThread* thread;
    FuriMutex* string_sync;
    FuriHalUsbInterface* usb_mode_prev;
} TotpTypeCodeWorkerContext;

typedef enum {
    TotpTypeCodeWorkerEvtReserved = (1 << 0),
    TotpTypeCodeWorkerEvtStop = (1 << 1),
    TotpTypeCodeWorkerEvtType = (1 << 2)
} TotpTypeCodeWorkerEvtFlags;

TotpTypeCodeWorkerContext* totp_type_code_worker_start();
void totp_type_code_worker_stop(TotpTypeCodeWorkerContext* context);
void totp_type_code_worker_notify(
    TotpTypeCodeWorkerContext* context,
    TotpTypeCodeWorkerEvtFlags event);