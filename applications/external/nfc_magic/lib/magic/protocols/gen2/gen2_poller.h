#pragma once

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_generic_event.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/nfc_device.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Gen2PollerErrorNone,
    Gen2PollerErrorNotPresent,
    Gen2PollerErrorProtocol,
    Gen2PollerErrorAuth,
    Gen2PollerErrorTimeout,
    Gen2PollerErrorAccess,
} Gen2PollerError;

// Possible write problems, sorted by priority top to bottom
typedef enum {
    Gen2PollerWriteProblemNoData, // Shouldn't happen, mfc_data missing in nfc device
    Gen2PollerWriteProblemLockedAccessBits, // Access bits on the target card don't allow writing in some cases
    Gen2PollerWriteProblemMissingTargetKeys, // Keys to write some sectors are not available
    Gen2PollerWriteProblemMissingSourceData, // The source dump is incomplete
    Gen2PollerWriteProblemNone, // Everything OK
} Gen2PollerWriteProblem;

typedef enum {
    Gen2PollerEventTypeDetected,
    Gen2PollerEventTypeRequestMode,
    Gen2PollerEventTypeRequestDataToWrite,
    Gen2PollerEventTypeRequestTargetData,

    Gen2PollerEventTypeSuccess,
    Gen2PollerEventTypeFail,
} Gen2PollerEventType;

typedef enum {
    Gen2PollerModeWipe,
    Gen2PollerModeWrite,
} Gen2PollerMode;

typedef struct {
    Gen2PollerMode mode;
} Gen2PollerEventDataRequestMode;

typedef struct {
    const MfClassicData* mfc_data;
} Gen2PollerEventDataRequestDataToWrite;

typedef struct {
    const MfClassicData* mfc_data;
} Gen2PollerEventDataRequestTargetData;

typedef union {
    Gen2PollerEventDataRequestMode poller_mode;
    Gen2PollerEventDataRequestDataToWrite data_to_write;
    Gen2PollerEventDataRequestTargetData target_data;
} Gen2PollerEventData;

typedef struct {
    Gen2PollerEventType type;
    Gen2PollerEventData* data;
} Gen2PollerEvent;

typedef NfcCommand (*Gen2PollerCallback)(Gen2PollerEvent event, void* context);

typedef struct Gen2Poller Gen2Poller;

Gen2PollerError gen2_poller_detect(Nfc* nfc);

Gen2Poller* gen2_poller_alloc(Nfc* nfc);

void gen2_poller_free(Gen2Poller* instance);

void gen2_poller_start(Gen2Poller* instance, Gen2PollerCallback callback, void* context);

void gen2_poller_stop(Gen2Poller* instance);

Gen2PollerWriteProblem gen2_poller_can_write_everything(NfcDevice* device);

#ifdef __cplusplus
}
#endif