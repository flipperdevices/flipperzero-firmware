#pragma once

#include <nfc/nfc.h>
#include <nfc/protocols/nfc_protocol.h>
#include <nfc/protocols/mf_classic/mf_classic.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight.h>

#ifdef __cplusplus
extern "C" {
#endif

#define GEN4_CMD_PREFIX (0xCF)
#define GEN4_CMD_GET_CFG (0xC6)
#define GEN4_CMD_WRITE (0xCD)
#define GEN4_CMD_READ (0xCE)
#define GEN4_CMD_SET_CFG (0xF0)
#define GEN4_CMD_FUSE_CFG (0xF1)
#define GEN4_CMD_SET_PWD (0xFE)

typedef enum {
    Gen4PollerErrorNone,
    Gen4PollerErrorTimeout,
    Gen4PollerErrorProtocol,
} Gen4PollerError;

typedef enum {
    Gen4PollerEventTypeCardDetected,
    Gen4PollerEventTypeRequestMode,
    Gen4PollerEventTypeRequestDataToWrite,
    Gen4PollerEventTypeRequestNewPassword,

    Gen4PollerEventTypeSuccess,
    Gen4PollerEventTypeFail,
} Gen4PollerEventType;

typedef enum {
    Gen4PollerModeWipe,
    Gen4PollerModeWrite,
    Gen4PollerModeSetPassword,

    Gen4PollerModeSetDefaultCfg,
    Gen4PollerModeGetCfg,
    Gen4PollerModeGetRevision,
    Gen4PollerModeSetShadowMode,
    Gen4PollerModeSetDirectWriteBlock0Mode
} Gen4PollerMode;

typedef struct {
    Gen4PollerMode mode;
} Gen4PollerEventDataRequestMode;

typedef struct {
    NfcProtocol protocol;
    const NfcDeviceData* data;
} Gen4PollerEventDataRequestDataToWrite;

typedef struct {
    uint32_t password;
} Gen4PollerEventDataRequestNewPassword;

typedef union {
    Gen4PollerEventDataRequestMode request_mode;
    Gen4PollerEventDataRequestDataToWrite request_data;
    Gen4PollerEventDataRequestNewPassword request_password;

    uint8_t display_config[32];
    uint8_t revision_data[5];
} Gen4PollerEventData;

typedef struct {
    Gen4PollerEventType type;
    Gen4PollerEventData* data;
} Gen4PollerEvent;

typedef NfcCommand (*Gen4PollerCallback)(Gen4PollerEvent event, void* context);

typedef struct Gen4Poller Gen4Poller;

Gen4PollerError gen4_poller_detect(Nfc* nfc, uint32_t password);

Gen4Poller* gen4_poller_alloc(Nfc* nfc);

void gen4_poller_free(Gen4Poller* instance);

void gen4_poller_set_password(Gen4Poller* instance, uint32_t password);

void gen4_poller_start(Gen4Poller* instance, Gen4PollerCallback callback, void* context);

void gen4_poller_stop(Gen4Poller* instance);

#ifdef __cplusplus
}
#endif
