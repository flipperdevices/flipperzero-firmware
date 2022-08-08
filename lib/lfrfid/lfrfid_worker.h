/**
 * @file lfrfid_worker.h
 * 
 * LFRFID worker
 */

#pragma once
#include <toolbox/protocols/protocol_dict.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LFRFIDWorkerWriteOK,
    LFRFIDWorkerWriteCannotWrite,
} LFRFIDWorkerWriteResult;

typedef enum {
    LFRFIDWorkerReadTypeAuto,
    LFRFIDWorkerReadTypeASKOnly,
    LFRFIDWorkerReadTypePSKOnly,
} LFRFIDWorkerReadType;

typedef enum {
    LFRFIDWorkerReadSenseStart, // TODO: not implemented
    LFRFIDWorkerReadSenseEnd, // TODO: not implemented
    LFRFIDWorkerReadDone,
} LFRFIDWorkerReadResult;

typedef enum {
    LFRFIDWorkerEmulateSense,
} LFRFIDWorkerEmulateResult;

typedef enum {
    LFRFIDWorkerReadRawFileError,
    LFRFIDWorkerReadRawOverrun,
    LFRFIDWorkerReadRawDone,
} LFRFIDWorkerReadRawResult;

typedef enum {
    LFRFIDWorkerEmulateRawFileError,
    LFRFIDWorkerEmulateRawOverrun,
    LFRFIDWorkerEmulateRawDone,
} LFRFIDWorkerEmulateRawResult;

typedef void (
    *LFRFIDWorkerReadCallback)(LFRFIDWorkerReadResult result, ProtocolId protocol, void* context);
typedef void (*LFRFIDWorkerWriteCallback)(LFRFIDWorkerWriteResult result, void* context);
typedef void (*LFRFIDWorkerEmulateCallback)(LFRFIDWorkerEmulateResult result, void* context);

typedef void (*LFRFIDWorkerReadRawCallback)(LFRFIDWorkerReadRawResult result, void* context);
typedef void (*LFRFIDWorkerEmulateRawCallback)(LFRFIDWorkerEmulateRawResult result, void* context);

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
 * Set "read event" callback
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
 * @param type 
 */
void lfrfid_worker_read_start(LFRFIDWorker* worker, LFRFIDWorkerReadType type);

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
 */
void lfrfid_worker_write_start(LFRFIDWorker* worker);

/**
 * Set "emulate event" callback
 * @param worker 
 * @param callback 
 * @param context 
 */
void lfrfid_worker_emulate_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerEmulateCallback callback,
    void* context);

/**
 * Start emulate mode
 * @param worker 
 */
void lfrfid_worker_emulate_start(LFRFIDWorker* worker);

/**
 * Set "read raw event" callback
 * @param worker 
 * @param callback 
 * @param context 
 */
void lfrfid_worker_read_raw_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerReadRawCallback callback,
    void* context);

/**
 * Start raw read mode
 * @param worker 
 * @param filename 
 * @param type 
 */
void lfrfid_worker_read_raw_start(
    LFRFIDWorker* worker,
    const char* filename,
    LFRFIDWorkerReadType type);

/**
 * Set "emulate raw event" callback
 * @param worker 
 * @param callback 
 * @param context 
 */
void lfrfid_worker_emulate_raw_set_callback(
    LFRFIDWorker* worker,
    LFRFIDWorkerEmulateRawCallback callback,
    void* context);

/**
 * Emulate raw read mode
 * @param worker 
 * @param filename 
 */
void lfrfid_worker_emulate_raw_start(LFRFIDWorker* worker, const char* filename);

/**
 * Stop all modes
 * @param worker 
 */
void lfrfid_worker_stop(LFRFIDWorker* worker);

size_t lfrfid_worker_dict_get_data_size(LFRFIDWorker* worker, ProtocolId protocol);

void lfrfid_worker_dict_get_data(
    LFRFIDWorker* worker,
    ProtocolId protocol,
    uint8_t* data,
    size_t data_size);

const char* lfrfid_worker_dict_get_name(LFRFIDWorker* worker, ProtocolId protocol);

const char* lfrfid_worker_dict_get_manufacturer(LFRFIDWorker* worker, ProtocolId protocol);

void lfrfid_worker_dict_render(LFRFIDWorker* worker, ProtocolId protocol, string_t result);

#ifdef __cplusplus
}
#endif
