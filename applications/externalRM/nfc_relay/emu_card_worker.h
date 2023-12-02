#pragma once
#include <furi.h>
#include <furi_hal_nfc.h>
#include "comm.h"

typedef enum {
    EmuCardWorkerStateStop,
    EmuCardWorkerStateWaitApduReq,
    EmuCardWorkerStateWaitApduResp,
    EmuCardWorkerStateGetApduResp,
} EmuCardWorkerState;

typedef struct EmuCardWorker {
    FuriThread* thread;
    FuriHalNfcDevData dev_data;
    EmuCardWorkerState state;
    uint32_t delayus;
    //AsCardWorker* as_card_wroker;
    //bool running;
    uint16_t apdu_buf_len;
    uint8_t apdu_buf[260];
} EmuCardWorker;

EmuCardWorker* emu_card_worker_alloc();
bool emu_card_worker_set_apdu(
    EmuCardWorker* emu_card_worker,
    uint8_t offset,
    uint8_t* data,
    uint16_t len);
bool emu_card_worker_get_apdu(EmuCardWorker* emu_card_worker, uint8_t* data, uint16_t* len);
void emu_card_worker_start(EmuCardWorker* emu_card_worker, FuriHalNfcDevData* dev_data);
void emu_card_worker_stop(EmuCardWorker* emu_card_worker);
void emu_card_worker_free(EmuCardWorker* emu_card_worker);
int32_t emu_card_worker_task(void* context);
