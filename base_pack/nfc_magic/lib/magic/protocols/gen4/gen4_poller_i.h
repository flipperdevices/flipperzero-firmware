#pragma once

#include "gen4_poller.h"
#include <nfc/nfc_poller.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>

#define TAG "Gen4Poller"

#ifdef __cplusplus
extern "C" {
#endif

#define GEN4_POLLER_MAX_BUFFER_SIZE (64U)
#define GEN4_POLLER_MAX_FWT (200000U)

#define GEN4_POLLER_BLOCK_SIZE (16)
#define GEN4_POLLER_BLOCKS_TOTAL (256)

#define GEN4_POLLER_CONFIG_SIZE_MAX (30)

typedef enum {
    Gen4PollerUIDLengthSingle = 0x00,
    Gen4PollerUIDLengthDouble = 0x01,
    Gen4PollerUIDLengthTriple = 0x02
} Gen4PollerUIDLength;

typedef enum {
    Gen4PollerUltralightModeUL_EV1 = 0x00,
    Gen4PollerUltralightModeNTAG = 0x01,
    Gen4PollerUltralightModeUL_C = 0x02,
    Gen4PollerUltralightModeUL = 0x03
} Gen4PollerUltralightMode;

typedef enum {
    // for writing original (shadow) data
    Gen4PollerShadowModePreWrite = 0x00,
    // written data can be read once before restored to original
    Gen4PollerShadowModeRestore = 0x01,
    // shadow mode disabled
    Gen4PollerShadowModeDisabled = 0x02,
    // apparently for UL?
    Gen4PollerShadowModeHighSpeedDisabled = 0x03,
    // work with new UMC. With old UMC is untested
    Gen4PollerShadowModeSplit = 0x04,
} Gen4PollerShadowMode;

typedef enum {
    // gen2 card behavour
    Gen4PollerDirectWriteBlock0ModeEnabled = 0x00,
    // common card behavour
    Gen4PollerDirectWriteBlock0ModeDisabled = 0x01,
    // default mode. same behavour as Gen4PollerDirectWriteBlock0ModeActivate
    Gen4PollerDirectWriteBlock0ModeDefault = 0x02,
} Gen4PollerDirectWriteBlock0Mode;

typedef enum {
    Gen4PollerStateIdle,
    Gen4PollerStateRequestMode,
    Gen4PollerStateRequestWriteData,
    Gen4PollerStateWrite,
    Gen4PollerStateWipe,
    Gen4PollerStateChangePassword,

    Gen4PollerStateSetDefaultConfig,
    Gen4PollerStateGetCurrentConfig,
    Gen4PollerStateGetRevision,
    Gen4PollerStateSetShadowMode,
    Gen4PollerStateSetDirectWriteBlock0,

    Gen4PollerStateSuccess,
    Gen4PollerStateFail,

    Gen4PollerStateNum,
} Gen4PollerState;

struct Gen4Poller {
    NfcPoller* poller;
    Iso14443_3aPoller* iso3_poller;
    Gen4PollerState state;
    uint32_t password;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    uint16_t current_block;
    uint16_t total_blocks;

    NfcProtocol protocol;
    const NfcDeviceData* data;
    uint32_t new_password;

    uint8_t config[GEN4_POLLER_CONFIG_SIZE_MAX];

    Gen4PollerShadowMode shadow_mode;
    Gen4PollerDirectWriteBlock0Mode direct_write_block_0_mode;

    Gen4PollerEvent gen4_event;
    Gen4PollerEventData gen4_event_data;

    Gen4PollerCallback callback;
    void* context;
};

Gen4PollerError gen4_poller_set_config(
    Gen4Poller* instance,
    uint32_t password,
    const uint8_t* config,
    size_t config_size,
    bool fuse);

Gen4PollerError gen4_poller_write_block(
    Gen4Poller* instance,
    uint32_t password,
    uint8_t block_num,
    const uint8_t* data);

Gen4PollerError
    gen4_poller_change_password(Gen4Poller* instance, uint32_t pwd_current, uint32_t pwd_new);

Gen4PollerError
    gen4_poller_get_revision(Gen4Poller* instance, uint32_t password, uint8_t* revision_result);

Gen4PollerError
    gen4_poller_get_config(Gen4Poller* instance, uint32_t password, uint8_t* config_result);

Gen4PollerError
    gen4_poller_set_shadow_mode(Gen4Poller* instance, uint32_t password, Gen4PollerShadowMode mode);

Gen4PollerError gen4_poller_set_direct_write_block_0_mode(
    Gen4Poller* instance,
    uint32_t password,
    Gen4PollerDirectWriteBlock0Mode mode);

#ifdef __cplusplus
}
#endif
