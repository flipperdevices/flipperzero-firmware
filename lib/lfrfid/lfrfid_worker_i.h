/**
 * @file lfrfid_worker_i.h
 * 
 * lfrfid worker, internal definitions 
 */

#pragma once
#include <furi.h>
#include "lfrfid_worker.h"
#include <toolbox/protocols/protocol_dict.h>
#include "protocols/lfrfid_protocols.h"
#include "lfrfid_raw_worker.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const uint32_t quant;
    void (*const start)(LFRFIDWorker* worker);
    void (*const tick)(LFRFIDWorker* worker);
    void (*const stop)(LFRFIDWorker* worker);
} LFRFIDWorkerModeType;

typedef enum {
    LFRFIDWorkerIdle,
    LFRFIDWorkerRead,
    LFRFIDWorkerWrite,
    LFRFIDWorkerEmulate,
    LFRFIDWorkerReadRaw,
    LFRFIDWorkerEmulateRaw,
} LFRFIDWorkerMode;

struct LFRFIDWorker {
    LFRFIDKey* key_p;
    uint8_t* key_data;
    char* raw_filename;
    // LFRFIDWriter* writer;

    LFRFIDWorkerMode mode_index;
    void* mode_storage;

    osMessageQueueId_t messages;
    FuriThread* thread;

    LFRFIDWorkerReadCallback read_cb;
    LFRFIDWorkerWriteCallback write_cb;
    LFRFIDWorkerEmulateCallback emulate_cb;
    LFRFIDWorkerReadRawCallback read_raw_cb;
    LFRFIDWorkerEmulateRawCallback emulate_raw_cb;

    void* cb_ctx;

    ProtocolDict* protocols;
    LFRFIDProtocol protocol_to_encode;

    LFRFIDRawWorker* raw_worker;
};

extern const LFRFIDWorkerModeType lfrfid_worker_modes[];

void lfrfid_worker_switch_mode(LFRFIDWorker* worker, LFRFIDWorkerMode mode);

#ifdef __cplusplus
}
#endif
