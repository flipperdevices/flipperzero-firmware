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
    float duty_cycle);

bool lfrfid_raw_worker_start_emulate(LFRFIDRawWorker* worker, const char* file_path);

bool lfrfid_raw_worker_stop(LFRFIDRawWorker* worker);

void lfrfid_raw_worker_emulate_set_callback(
    LFRFIDRawWorker* worker,
    LFRFIDWorkerEmulateRawCallback callback,
    void* context);

void lfrfid_raw_worker_read_set_callback(
    LFRFIDRawWorker* worker,
    LFRFIDWorkerReadRawCallback callback,
    void* context);

#ifdef __cplusplus
}
#endif