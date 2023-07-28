#pragma once

#include "ublox_worker.h"
#include "ublox_i.h"

#include <furi.h>
#include <furi_hal.h>

struct UbloxWorker {
    FuriThread* thread;
    FuriTimer* timer;

    UbloxWorkerCallback callback;
    void* context;

    UbloxWorkerState state;
};

void ublox_worker_change_state(UbloxWorker* ublox_worker, UbloxWorkerState state);

int32_t ublox_worker_task(void* context);

void ublox_worker_read_pvt(UbloxWorker* ublox_worker);

bool ublox_worker_read_odo(UbloxWorker* ublox_worker);

void ublox_worker_reset_odo(UbloxWorker* ublox_worker);
