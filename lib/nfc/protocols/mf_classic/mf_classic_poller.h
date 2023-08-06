#pragma once

#include "mf_classic.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicPoller MfClassicPoller;

typedef enum {
    // Start event
    MfClassicPollerEventTypeRequestMode,

    // Read with key cache events
    MfClassicPollerEventTypeRequestReadSector,

    // Write events
    MfClassicPollerEventTypeRequestSectorTrailer,
    MfClassicPollerEventTypeRequestWriteBlock,

    // Dictionary attack events
    MfClassicPollerEventTypeRequestKey,
    MfClassicPollerEventTypeNewSector,
    MfClassicPollerEventTypeFoundKeyA,
    MfClassicPollerEventTypeFoundKeyB,
    MfClassicPollerEventTypeCardNotDetected,
    MfClassicPollerEventTypeKeyAttackStart,
    MfClassicPollerEventTypeKeyAttackStop,
    MfClassicPollerEventTypeKeyAttackNextSector,

    // Common events
    MfClassicPollerEventTypeCardDetected,
    MfClassicPollerEventTypeCardLost,
    MfClassicPollerEventTypeSuccess,
    MfClassicPollerEventTypeFail,
} MfClassicPollerEventType;

typedef enum {
    MfClassicPollerModeRead,
    MfClassicPollerModeWrite,
    MfClassicPollerModeDictAttack,
} MfClassicPollerMode;

typedef struct {
    MfClassicPollerMode mode;
} MfClassicPollerEventDataRequestMode;

typedef struct {
    uint8_t sector_num;
    MfClassicKey key;
    bool key_provided;
} MfClassicPollerEventDataKeyRequest;

typedef struct {
    uint8_t sector_num;
    MfClassicKey key;
    MfClassicKeyType key_type;
    bool key_provided;
} MfClassicPollerEventDataReadSectorRequest;

typedef struct {
    uint8_t sector_num;
    MfClassicBlock sector_trailer;
    bool sector_trailer_provided;
} MfClassicPollerEventDataSectorTrailerRequest;

typedef struct {
    uint8_t block_num;
    MfClassicBlock write_block;
    bool write_block_provided;
} MfClassicPollerEventDataWriteBlockRequest;

typedef struct {
    uint8_t start_sector;
} MfClassicPollerEventKeyAttackData;

typedef union {
    MfClassicError error;
    MfClassicPollerEventDataRequestMode poller_mode;
    MfClassicPollerEventDataKeyRequest key_request_data;
    MfClassicPollerEventDataReadSectorRequest read_sector_request_data;
    MfClassicPollerEventKeyAttackData key_attack_data;
    MfClassicPollerEventDataSectorTrailerRequest sec_tr_data;
    MfClassicPollerEventDataWriteBlockRequest write_block_data;
} MfClassicPollerEventData;

typedef struct {
    MfClassicPollerEventType type;
    MfClassicPollerEventData* data;
} MfClassicPollerEvent;

#ifdef __cplusplus
}
#endif
