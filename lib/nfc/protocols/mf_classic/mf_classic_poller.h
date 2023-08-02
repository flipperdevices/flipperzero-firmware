#pragma once

#include "mf_classic.h"
#include <lib/nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct MfClassicPoller MfClassicPoller;

typedef enum {
    MfClassicPollerEventTypeRequestMode,
    MfClassicPollerEventTypeRequestKey,
    MfClassicPollerEventTypeRequestReadSector,
    MfClassicPollerEventTypeNewSector,
    MfClassicPollerEventTypeFoundKeyA,
    MfClassicPollerEventTypeFoundKeyB,
    MfClassicPollerEventTypeCardDetected,
    MfClassicPollerEventTypeCardNotDetected,
    MfClassicPollerEventTypeKeyAttackStart,
    MfClassicPollerEventTypeKeyAttackStop,
    MfClassicPollerEventTypeKeyAttackNextSector,
    MfClassicPollerEventTypeReadComplete,
} MfClassicPollerEventType;

typedef enum {
    MfClassicPollerModeDictAttack,
    MfClassicPollerModeKeyCache,
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
    uint8_t start_sector;
} MfClassicPollerEventKeyAttackData;

typedef union {
    MfClassicError error;
    MfClassicPollerEventDataRequestMode poller_mode;
    MfClassicPollerEventDataKeyRequest key_request_data;
    MfClassicPollerEventDataReadSectorRequest read_sector_request_data;
    MfClassicPollerEventKeyAttackData key_attack_data;
} MfClassicPollerEventData;

typedef struct {
    MfClassicPollerEventType type;
    MfClassicPollerEventData* data;
} MfClassicPollerEvent;

#ifdef __cplusplus
}
#endif
