#pragma once

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_generic_event.h>
#include <nfc/protocols/mf_classic/mf_classic.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Gen1aPollerEventTypeDetected,
    Gen1aPollerEventTypeRequestMode,
    Gen1aPollerEventTypeRequestDataToWrite,

    Gen1aPollerEventTypeSuccess,
    Gen1aPollerEventTypeFail,
} Gen1aPollerEventType;

typedef enum {
    Gen1aPollerModeWipe,
    Gen1aPollerModeWrite,
} Gen1aPollerMode;

typedef struct {
    Gen1aPollerMode mode;
} Gen1aPollerEventDataRequestMode;

typedef struct {
    const MfClassicData* mfc_data;
} Gen1aPollerEventDataRequestDataToWrite;

typedef union {
    Gen1aPollerEventDataRequestMode request_mode;
    Gen1aPollerEventDataRequestDataToWrite data_to_write;
} Gen1aPollerEventData;

typedef struct {
    Gen1aPollerEventType type;
    Gen1aPollerEventData* data;
} Gen1aPollerEvent;

typedef NfcCommand (*Gen1aPollerCallback)(Gen1aPollerEvent event, void* context);

typedef struct Gen1aPoller Gen1aPoller;

bool gen1a_poller_detect(Nfc* nfc);

Gen1aPoller* gen1a_poller_alloc(Nfc* nfc);

void gen1a_poller_free(Gen1aPoller* instance);

void gen1a_poller_start(Gen1aPoller* instance, Gen1aPollerCallback callback, void* context);

void gen1a_poller_stop(Gen1aPoller* instance);

#ifdef __cplusplus
}
#endif
