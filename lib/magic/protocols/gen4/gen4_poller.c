#include "gen4_poller_i.h"
#include "protocols/gen4/gen4_poller.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/helpers/nfc_util.h>
#include <nfc/nfc_poller.h>

#define GEN4_POLLER_THREAD_FLAG_DETECTED (1U << 0)

typedef NfcCommand (*Gen4PollerStateHandler)(Gen4Poller* instance);

typedef struct {
    NfcPoller* poller;
    uint32_t password;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    FuriThreadId thread_id;
    Gen4PollerError error;
} Gen4PollerDetectContext;

static const uint8_t gen4_poller_default_config[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02,
                                                     0x00, 0x09, 0x78, 0x00, 0x91, 0x02, 0xDA,
                                                     0xBC, 0x19, 0x10, 0x10, 0x11, 0x12, 0x13,
                                                     0x14, 0x15, 0x16, 0x04, 0x00, 0x08, 0x00};
static const uint8_t gen4_poller_default_block_0[GEN4_POLLER_BLOCK_SIZE] =
    {0x00, 0x01, 0x02, 0x03, 0x04, 0x04, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const uint8_t gen4_poller_default_empty_block[GEN4_POLLER_BLOCK_SIZE] = {0};

static const uint8_t gen4_poller_default_sector_trailer_block[GEN4_POLLER_BLOCK_SIZE] =
    {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x07, 0x80, 0x69, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static bool gen4_poller_is_sector_trailer(uint8_t block_num) {
    uint8_t sec_tr_block_num = 0;

    if(block_num < 128) {
        sec_tr_block_num = block_num | 0x03;
    } else {
        sec_tr_block_num = block_num | 0x0f;
    }

    return block_num == sec_tr_block_num;
}

Gen4Poller* gen4_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    Gen4Poller* instance = malloc(sizeof(Gen4Poller));
    instance->poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a);

    instance->gen4_event.data = &instance->gen4_event_data;

    instance->tx_buffer = bit_buffer_alloc(GEN4_POLLER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(GEN4_POLLER_MAX_BUFFER_SIZE);

    return instance;
}

void gen4_poller_free(Gen4Poller* instance) {
    furi_assert(instance);

    nfc_poller_free(instance->poller);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);

    free(instance);
}

void gen4_poller_set_password(Gen4Poller* instance, uint32_t password) {
    furi_assert(instance);

    instance->password = password;
}

NfcCommand gen4_poller_detect_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.instance);
    furi_assert(event.event_data);

    NfcCommand command = NfcCommandStop;
    Gen4PollerDetectContext* gen4_poller_detect_ctx = context;
    Iso14443_3aPoller* iso3_poller = event.instance;
    Iso14443_3aPollerEvent* iso3_event = event.event_data;
    gen4_poller_detect_ctx->error = Gen4PollerErrorTimeout;

    if(iso3_event->type == Iso14443_3aPollerEventTypeReady) {
        do {
            bit_buffer_append_byte(gen4_poller_detect_ctx->tx_buffer, GEN4_CMD_PREFIX);
            uint8_t pwd_arr[4] = {};
            nfc_util_num2bytes(gen4_poller_detect_ctx->password, COUNT_OF(pwd_arr), pwd_arr);
            bit_buffer_append_bytes(gen4_poller_detect_ctx->tx_buffer, pwd_arr, COUNT_OF(pwd_arr));
            bit_buffer_append_byte(gen4_poller_detect_ctx->tx_buffer, GEN4_CMD_GET_CFG);

            Iso14443_3aError error = iso14443_3a_poller_send_standard_frame(
                iso3_poller,
                gen4_poller_detect_ctx->tx_buffer,
                gen4_poller_detect_ctx->rx_buffer,
                GEN4_POLLER_MAX_FWT);

            if(error != Iso14443_3aErrorNone) {
                gen4_poller_detect_ctx->error = Gen4PollerErrorProtocol;
                break;
            }
            size_t rx_bytes = bit_buffer_get_size_bytes(gen4_poller_detect_ctx->rx_buffer);
            if((rx_bytes != 30) && (rx_bytes != 32)) {
                gen4_poller_detect_ctx->error = Gen4PollerErrorProtocol;
                break;
            }

            gen4_poller_detect_ctx->error = Gen4PollerErrorNone;
        } while(false);
    } else if(iso3_event->type == Iso14443_3aPollerEventTypeError) {
        gen4_poller_detect_ctx->error = Gen4PollerErrorTimeout;
    }
    furi_thread_flags_set(gen4_poller_detect_ctx->thread_id, GEN4_POLLER_THREAD_FLAG_DETECTED);

    return command;
}

Gen4PollerError gen4_poller_detect(Nfc* nfc, uint32_t password) {
    furi_assert(nfc);

    Gen4PollerDetectContext gen4_poller_detect_ctx = {};
    gen4_poller_detect_ctx.poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a);
    gen4_poller_detect_ctx.password = password;
    gen4_poller_detect_ctx.tx_buffer = bit_buffer_alloc(GEN4_POLLER_MAX_BUFFER_SIZE);
    gen4_poller_detect_ctx.rx_buffer = bit_buffer_alloc(GEN4_POLLER_MAX_BUFFER_SIZE);
    gen4_poller_detect_ctx.thread_id = furi_thread_get_current_id();
    gen4_poller_detect_ctx.error = Gen4PollerErrorNone;

    nfc_poller_start(
        gen4_poller_detect_ctx.poller, gen4_poller_detect_callback, &gen4_poller_detect_ctx);
    uint32_t flags =
        furi_thread_flags_wait(GEN4_POLLER_THREAD_FLAG_DETECTED, FuriFlagWaitAny, FuriWaitForever);
    if(flags & GEN4_POLLER_THREAD_FLAG_DETECTED) {
        furi_thread_flags_clear(GEN4_POLLER_THREAD_FLAG_DETECTED);
    }
    nfc_poller_stop(gen4_poller_detect_ctx.poller);

    nfc_poller_free(gen4_poller_detect_ctx.poller);
    bit_buffer_free(gen4_poller_detect_ctx.tx_buffer);
    bit_buffer_free(gen4_poller_detect_ctx.rx_buffer);

    return gen4_poller_detect_ctx.error;
}

NfcCommand gen4_poller_idle_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->current_block = 0;
    memset(instance->config, 0, sizeof(instance->config));
    instance->gen4_event.type = Gen4PollerEventTypeCardDetected;
    command = instance->callback(instance->gen4_event, instance->context);
    instance->state = Gen4PollerStateRequestMode;

    return command;
}

NfcCommand gen4_poller_request_mode_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen4_event.type = Gen4PollerEventTypeRequestMode;
    command = instance->callback(instance->gen4_event, instance->context);
    if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeWipe) {
        instance->state = Gen4PollerStateWipe;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeWrite) {
        instance->state = Gen4PollerStateRequestWriteData;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeSetPassword) {
        instance->state = Gen4PollerStateChangePassword;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeSetDefaultCfg) {
        instance->state = Gen4PollerStateSetDefaultConfig;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeGetCfg) {
        instance->state = Gen4PollerStateGetCurrentConfig;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeGetRevision) {
        instance->state = Gen4PollerStateGetRevision;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeSetShadowMode) {
        instance->state = Gen4PollerStateSetShadowMode;
    } else if(instance->gen4_event_data.request_mode.mode == Gen4PollerModeSetDirectWriteBlock0Mode) {
        instance->state = Gen4PollerStateSetDirectWriteBlock0;
    } else {
        instance->state = Gen4PollerStateFail;
    }

    return command;
}

NfcCommand gen4_poller_wipe_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        Gen4PollerError error = Gen4PollerErrorNone;
        if(instance->current_block == 0) {
            error = gen4_poller_set_config(
                instance,
                instance->password,
                gen4_poller_default_config,
                sizeof(gen4_poller_default_config),
                false);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to set default config: %d", error);
                instance->state = Gen4PollerStateFail;
                break;
            }
            instance->password = 0;
            error = gen4_poller_write_block(
                instance, instance->password, instance->current_block, gen4_poller_default_block_0);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write 0 block: %d", error);
                instance->state = Gen4PollerStateFail;
                break;
            }
        } else if(instance->current_block < GEN4_POLLER_BLOCKS_TOTAL) {
            const uint8_t* block = gen4_poller_is_sector_trailer(instance->current_block) ?
                                       gen4_poller_default_sector_trailer_block :
                                       gen4_poller_default_empty_block;
            error = gen4_poller_write_block(
                instance, instance->password, instance->current_block, block);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write %d block: %d", instance->current_block, error);
                instance->state = Gen4PollerStateFail;
                break;
            }
        } else {
            instance->state = Gen4PollerStateSuccess;
            break;
        }
        instance->current_block++;
    } while(false);

    return command;
}

NfcCommand gen4_poller_request_write_data_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen4_event.type = Gen4PollerEventTypeRequestDataToWrite;
    command = instance->callback(instance->gen4_event, instance->context);
    instance->protocol = instance->gen4_event_data.request_data.protocol;
    instance->data = instance->gen4_event_data.request_data.data;

    if((instance->protocol == NfcProtocolMfClassic) ||
       (instance->protocol == NfcProtocolMfUltralight)) {
        instance->state = Gen4PollerStateWrite;
    } else {
        FURI_LOG_E(TAG, "Unsupported protocol");
        instance->state = Gen4PollerStateFail;
    }

    return command;
}

static NfcCommand gen4_poller_write_mf_classic(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        const MfClassicData* mfc_data = instance->data;
        const Iso14443_3aData* iso3_data = mfc_data->iso14443_3a_data;
        if(instance->current_block == 0) {
            instance->config[0] = 0x00;
            instance->total_blocks = mf_classic_get_total_block_num(mfc_data->type);

            if(iso3_data->uid_len == 4) {
                instance->config[1] = Gen4PollerUIDLengthSingle;
            } else if(iso3_data->uid_len == 7) {
                instance->config[1] = Gen4PollerUIDLengthDouble;
            } else {
                FURI_LOG_E(TAG, "Unsupported UID len: %d", iso3_data->uid_len);
                instance->state = Gen4PollerStateFail;
                break;
            }

            instance->config[6] = Gen4PollerShadowModeDisabled;
            instance->config[24] = iso3_data->atqa[0];
            instance->config[25] = iso3_data->atqa[1];
            instance->config[26] = iso3_data->sak;
            instance->config[27] = 0x00;
            instance->config[28] = instance->total_blocks - 1;
            instance->config[29] = Gen4PollerDirectWriteBlock0ModeDisabled;

            Gen4PollerError error = gen4_poller_set_config(
                instance, instance->password, instance->config, sizeof(instance->config), false);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write config: %d", error);
                instance->state = Gen4PollerStateFail;
                break;
            }
        }
        if(instance->current_block < instance->total_blocks) {
            FURI_LOG_D(TAG, "Writing block %d", instance->current_block);
            Gen4PollerError error = gen4_poller_write_block(
                instance,
                instance->password,
                instance->current_block,
                mfc_data->block[instance->current_block].data);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write %d block: %d", instance->current_block, error);
                instance->state = Gen4PollerStateFail;
                break;
            }
        } else {
            instance->state = Gen4PollerStateSuccess;
            break;
        }
        instance->current_block++;
    } while(false);

    return command;
}

static NfcCommand gen4_poller_write_mf_ultralight(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        const MfUltralightData* mfu_data = instance->data;
        const Iso14443_3aData* iso3_data = mfu_data->iso14443_3a_data;
        if(instance->current_block == 0) {
            instance->total_blocks = 64;
            instance->config[0] = 0x01;
            switch(mfu_data->type) {
            case MfUltralightTypeNTAG203:
            case MfUltralightTypeNTAG213:
            case MfUltralightTypeNTAG215:
            case MfUltralightTypeNTAG216:
            case MfUltralightTypeNTAGI2C1K:
            case MfUltralightTypeNTAGI2C2K:
            case MfUltralightTypeNTAGI2CPlus1K:
            case MfUltralightTypeNTAGI2CPlus2K:
                instance->config[27] = Gen4PollerUltralightModeNTAG;
                instance->total_blocks = 64 * 2;
                break;

            case MfUltralightTypeUL11:
            case MfUltralightTypeUL21:
                // UL-C?
                // UL?
            default:
                instance->config[27] = Gen4PollerUltralightModeUL_EV1;
                break;
            }

            if(iso3_data->uid_len == 4) {
                instance->config[1] = Gen4PollerUIDLengthSingle;
            } else if(iso3_data->uid_len == 7) {
                instance->config[1] = Gen4PollerUIDLengthDouble;
            } else {
                FURI_LOG_E(TAG, "Unsupported UID len: %d", iso3_data->uid_len);
                instance->state = Gen4PollerStateFail;
                break;
            }

            instance->config[6] = Gen4PollerShadowModeHighSpeedDisabled;
            instance->config[24] = iso3_data->atqa[0];
            instance->config[25] = iso3_data->atqa[1];
            instance->config[26] = iso3_data->sak;
            instance->config[27] = 0x00;
            instance->config[28] = instance->total_blocks - 1;
            instance->config[29] = Gen4PollerDirectWriteBlock0ModeDisabled;

            Gen4PollerError error = gen4_poller_set_config(
                instance, instance->password, instance->config, sizeof(instance->config), false);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write config: %d", error);
                instance->state = Gen4PollerStateFail;
                break;
            }
        }

        if(instance->current_block < mfu_data->pages_read) {
            FURI_LOG_D(
                TAG, "Writing page %zu / %zu", instance->current_block, mfu_data->pages_read);
            Gen4PollerError error = gen4_poller_write_block(
                instance,
                instance->password,
                instance->current_block,
                mfu_data->page[instance->current_block].data);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_D(TAG, "Failed to write %d page: %d", instance->current_block, error);
                instance->state = Gen4PollerStateFail;
                break;
            }
            instance->current_block++;
        } else {
            uint8_t block[GEN4_POLLER_BLOCK_SIZE] = {};
            bool write_success = true;
            for(size_t i = 0; i < 8; i++) {
                memcpy(block, &mfu_data->signature.data[i * 4], 4); //-V1086
                Gen4PollerError error =
                    gen4_poller_write_block(instance, instance->password, 0xF2 + i, block);
                if(error != Gen4PollerErrorNone) {
                    write_success = false;
                    break;
                }
            }
            if(!write_success) {
                FURI_LOG_E(TAG, "Failed to write Signature");
                instance->state = Gen4PollerStateFail;
                break;
            }

            block[0] = mfu_data->version.header;
            block[1] = mfu_data->version.vendor_id;
            block[2] = mfu_data->version.prod_type;
            block[3] = mfu_data->version.prod_subtype;
            Gen4PollerError error =
                gen4_poller_write_block(instance, instance->password, 0xFA, block);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_E(TAG, "Failed to write 1st part Version");
                instance->state = Gen4PollerStateFail;
                break;
            }

            block[0] = mfu_data->version.prod_ver_major;
            block[1] = mfu_data->version.prod_ver_minor;
            block[2] = mfu_data->version.storage_size;
            block[3] = mfu_data->version.protocol_type;
            error = gen4_poller_write_block(instance, instance->password, 0xFB, block);
            if(error != Gen4PollerErrorNone) {
                FURI_LOG_E(TAG, "Failed to write 2nd part Version");
                instance->state = Gen4PollerStateFail;
                break;
            }

            instance->state = Gen4PollerStateSuccess;
        }
    } while(false);

    return command;
}

NfcCommand gen4_poller_write_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    memcpy(instance->config, gen4_poller_default_config, sizeof(gen4_poller_default_config));
    uint8_t password_arr[4] = {};
    nfc_util_num2bytes(instance->password, sizeof(password_arr), password_arr);
    memcpy(&instance->config[2], password_arr, sizeof(password_arr));
    memset(&instance->config[7], 0, 17);
    if(instance->protocol == NfcProtocolMfClassic) {
        command = gen4_poller_write_mf_classic(instance);
    } else if(instance->protocol == NfcProtocolMfUltralight) {
        command = gen4_poller_write_mf_ultralight(instance);
    } else {
        furi_crash("Unsupported protocol to write");
    }

    return command;
}

NfcCommand gen4_poller_change_password_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        instance->gen4_event.type = Gen4PollerEventTypeRequestNewPassword;
        command = instance->callback(instance->gen4_event, instance->context);
        if(command != NfcCommandContinue) break;

        uint32_t new_password = instance->gen4_event_data.request_password.password;
        Gen4PollerError error =
            gen4_poller_change_password(instance, instance->password, new_password);
        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to change password: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }

        instance->password = new_password;
        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_set_default_cfg_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        Gen4PollerError error = gen4_poller_set_config(
            instance,
            instance->password,
            gen4_poller_default_config,
            sizeof(gen4_poller_default_config),
            false);
        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to set default config: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }

        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_get_current_cfg_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        uint8_t the_config[32] = {};

        Gen4PollerError error = gen4_poller_get_config(instance, instance->password, the_config);
        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to get current config: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }
        // Copy config data to event data buffer
        memcpy(instance->gen4_event_data.display_config, the_config, sizeof(the_config));

        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_get_revision_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        uint8_t the_revision[5] = {0};
        Gen4PollerError error =
            gen4_poller_get_revision(instance, instance->password, the_revision);
        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to get revision: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }
        // Copy revision data to event data buffer
        memcpy(instance->gen4_event_data.revision_data, the_revision, sizeof(the_revision));

        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_set_shadow_mode_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        Gen4PollerError error =
            gen4_poller_set_shadow_mode(instance, instance->password, instance->shadow_mode);

        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to set shadow mode: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }

        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_set_direct_write_block_0_mode_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    do {
        Gen4PollerError error = gen4_poller_set_direct_write_block_0_mode(
            instance, instance->password, instance->direct_write_block_0_mode);

        if(error != Gen4PollerErrorNone) {
            FURI_LOG_E(TAG, "Failed to set direct write to block 0 mode: %d", error);
            instance->state = Gen4PollerStateFail;
            break;
        }

        instance->state = Gen4PollerStateSuccess;
    } while(false);

    return command;
}

NfcCommand gen4_poller_success_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen4_event.type = Gen4PollerEventTypeSuccess;
    command = instance->callback(instance->gen4_event, instance->context);
    if(command != NfcCommandStop) {
        furi_delay_ms(100);
    }

    return command;
}

NfcCommand gen4_poller_fail_handler(Gen4Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen4_event.type = Gen4PollerEventTypeFail;
    command = instance->callback(instance->gen4_event, instance->context);
    if(command != NfcCommandStop) {
        furi_delay_ms(100);
    }

    return command;
}

static const Gen4PollerStateHandler gen4_poller_state_handlers[Gen4PollerStateNum] = {
    [Gen4PollerStateIdle] = gen4_poller_idle_handler,
    [Gen4PollerStateRequestMode] = gen4_poller_request_mode_handler,
    [Gen4PollerStateRequestWriteData] = gen4_poller_request_write_data_handler,
    [Gen4PollerStateWrite] = gen4_poller_write_handler,
    [Gen4PollerStateWipe] = gen4_poller_wipe_handler,
    [Gen4PollerStateChangePassword] = gen4_poller_change_password_handler,
    [Gen4PollerStateSetDefaultConfig] = gen4_poller_set_default_cfg_handler,
    [Gen4PollerStateGetCurrentConfig] = gen4_poller_get_current_cfg_handler,
    [Gen4PollerStateGetRevision] = gen4_poller_get_revision_handler,
    [Gen4PollerStateSetShadowMode] = gen4_poller_set_shadow_mode_handler,
    [Gen4PollerStateSetDirectWriteBlock0] = gen4_poller_set_direct_write_block_0_mode_handler,
    [Gen4PollerStateSuccess] = gen4_poller_success_handler,
    [Gen4PollerStateFail] = gen4_poller_fail_handler,

};

static NfcCommand gen4_poller_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.event_data);
    furi_assert(event.instance);

    NfcCommand command = NfcCommandContinue;
    Gen4Poller* instance = context;
    instance->iso3_poller = event.instance;
    Iso14443_3aPollerEvent* iso3_event = event.event_data;

    if(iso3_event->type == Iso14443_3aPollerEventTypeReady) {
        command = gen4_poller_state_handlers[instance->state](instance);
    }

    return command;
}

void gen4_poller_start(Gen4Poller* instance, Gen4PollerCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    nfc_poller_start(instance->poller, gen4_poller_callback, instance);
}

void gen4_poller_stop(Gen4Poller* instance) {
    furi_assert(instance);

    nfc_poller_stop(instance->poller);
}
