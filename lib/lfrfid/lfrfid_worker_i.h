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
    LFRFIDWorkerIdle = 0,
    LFRFIDWorkerRead = 1,
    LFRFIDWorkerWrite = 2,
    LFRFIDWorkerEmulate = 3,
} LFRFIDWorkerMode;

struct LFRFIDWorker {
    LFRFIDKey* key_p;
    uint8_t* key_data;
    // LFRFIDWriter* writer;
    LFRFIDWorkerMode mode_index;
    osMessageQueueId_t messages;
    FuriThread* thread;

    LFRFIDWorkerReadCallback read_cb;
    LFRFIDWorkerWriteCallback write_cb;
    void* cb_ctx;

    ProtocolDict* protocols;
    LFRFIDProtocol protocol_to_encode;
};

extern const LFRFIDWorkerModeType lfrfid_worker_modes[];

void lfrfid_worker_switch_mode(LFRFIDWorker* worker, LFRFIDWorkerMode mode);
void lfrfid_worker_notify_emulate(LFRFIDWorker* worker);

#ifdef __cplusplus
}
#endif
