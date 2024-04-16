#pragma once

#include "gen4_poller.h"
#include <nfc/nfc_poller.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <bit_lib/bit_lib.h>

#define TAG "Gen4Poller"

#ifdef __cplusplus
extern "C" {
#endif

#define GEN4_POLLER_MAX_BUFFER_SIZE (64U)
#define GEN4_POLLER_MAX_FWT (200000U)

#define GEN4_POLLER_BLOCK_SIZE (16)
#define GEN4_POLLER_BLOCKS_TOTAL (256)

typedef enum {
    Gen4PollerStateIdle,
    Gen4PollerStateRequestMode,
    Gen4PollerStateRequestWriteData,
    Gen4PollerStateWrite,
    Gen4PollerStateWipe,
    Gen4PollerStateChangePassword,

    Gen4PollerStateGetInfo,
    Gen4PollerStateSetDefaultConfig,
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

    Gen4* gen4_data;

    Gen4Password password;

    Gen4Password new_password;
    Gen4Config config;
    Gen4ShadowMode shadow_mode;
    Gen4DirectWriteBlock0Mode direct_write_block_0_mode;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;

    uint16_t current_block;
    uint16_t total_blocks;

    NfcProtocol protocol;
    const NfcDeviceData* data;

    Gen4PollerEvent gen4_event;
    Gen4PollerEventData gen4_event_data;

    Gen4PollerCallback callback;
    void* context;
};

Gen4PollerError gen4_poller_set_config(
    Gen4Poller* instance,
    Gen4Password password,
    const Gen4Config* config,
    size_t config_size,
    bool fuse);

Gen4PollerError gen4_poller_write_block(
    Gen4Poller* instance,
    Gen4Password password,
    uint8_t block_num,
    const uint8_t* data);

Gen4PollerError gen4_poller_change_password(
    Gen4Poller* instance,
    Gen4Password pwd_current,
    Gen4Password pwd_new);

Gen4PollerError gen4_poller_get_revision(
    Gen4Poller* instance,
    Gen4Password password,
    Gen4Revision* revision_result);

Gen4PollerError
    gen4_poller_get_config(Gen4Poller* instance, Gen4Password password, Gen4Config* config_result);

Gen4PollerError
    gen4_poller_set_shadow_mode(Gen4Poller* instance, Gen4Password password, Gen4ShadowMode mode);

Gen4PollerError gen4_poller_set_direct_write_block_0_mode(
    Gen4Poller* instance,
    Gen4Password password,
    Gen4DirectWriteBlock0Mode mode);

#ifdef __cplusplus
}
#endif
