/**
 * @file lfrfid_worker.h
 * 
 * LFRFID worker
 */

#pragma once
#include "lfrfid_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LFRFIDWorkerWriteOK,
    LFRFIDWorkerWriteSameKey,
    LFRFIDWorkerWriteNoDetect,
    LFRFIDWorkerWriteCannotWrite,
} LFRFIDWorkerWriteResult;

typedef void (*LFRFIDWorkerReadCallback)(void* context);
typedef void (*LFRFIDWorkerWriteCallback)(void* context, LFRFIDWorkerWriteResult result);

typedef struct LFRFIDWorker LFRFIDWorker;

/**
 * Allocate LF-RFID worker
 * @return LFRFIDWorker* 
 */
LFRFIDWorker* lfrfid_worker_alloc();

/**
 * Free LF-RFID worker
 * @param worker 
 */
void lfrfid_worker_free(LFRFIDWorker* worker);

/**
 * Start LF-RFID worker thread
 * @param worker 
 */
void lfrfid_worker_start_thread(LFRFIDWorker* worker);

/**
 * Stop LF-RFID worker thread
 * @param worker 
 */
void lfrfid_worker_stop_thread(LFRFIDWorker* worker);

/**
 * Set "read success" callback
 * @param worker 
 * @param callback 
 * @param context 
 */
void lfrfid_worker_read_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerReadCallback callback,
    void* context);

/**
 * Start read mode
 * @param worker 
 * @param key 
 */
void lfrfid_worker_read_start(LFRFIDWorker* worker, LFRFIDKey* key);

/**
 * Set "write event" callback
 * @param worker 
 * @param callback 
 * @param context 
 */
void lfrfid_worker_write_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerWriteCallback callback,
    void* context);

/**
 * Start write mode
 * @param worker 
 * @param key 
 */
void lfrfid_worker_write_start(LFRFIDWorker* worker, LFRFIDKey* key);

/**
 * Start emulate mode
 * @param worker 
 * @param key 
 */
void lfrfid_worker_emulate_start(LFRFIDWorker* worker, LFRFIDKey* key);

/**
 * Stop all modes
 * @param worker 
 */
void lfrfid_worker_stop(LFRFIDWorker* worker);

#ifdef __cplusplus
}
#endif
