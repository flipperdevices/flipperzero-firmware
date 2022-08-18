#pragma once
#include <furi.h>
#include "lfrfid_worker.h"
#include <toolbox/protocols/protocol_dict.h>
#include "protocols/lfrfid_protocols.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LFRFIDRawWorker LFRFIDRawWorker;

LFRFIDRawWorker* lfrfid_raw_worker_alloc();

void lfrfid_raw_worker_free(LFRFIDRawWorker* worker);

bool lfrfid_raw_worker_start_read(
    LFRFIDRawWorker* worker,
    const char* file_path,
    float frequency,
    float duty_cycle,
    LFRFIDWorkerReadRawCallback callback,
    void* context);

void lfrfid_raw_worker_start_emulate(
    LFRFIDRawWorker* worker,
    const char* file_path,
    LFRFIDWorkerEmulateRawCallback callback,
    void* context);

void lfrfid_raw_worker_stop(LFRFIDRawWorker* worker);

#ifdef __cplusplus
}
#endif