#include "gen2_poller_i.h"
#include <nfc/helpers/nfc_data_generator.h>

#include <furi/furi.h>

#define GEN2_POLLER_THREAD_FLAG_DETECTED (1U << 0)

#define TAG "GEN2"

typedef NfcCommand (*Gen2PollerStateHandler)(Gen2Poller* instance);

typedef struct {
    NfcPoller* poller;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    FuriThreadId thread_id;
    bool detected;
    Gen2PollerError error;
} Gen2PollerDetectContext;

// Array of known Gen2 ATS responses
// 0978009102DABC1910F005 - flavour 2
// 0978009102DABC1910F005 - flavour 4
// 0D780071028849A13020150608563D - flavour 6
// Other flavours can't be detected other than by just trying to write to block 0
const uint8_t GEN2_ATS[3][16] = {
    {0x09, 0x78, 0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0xF0, 0x05},
    {0x09, 0x78, 0x00, 0x91, 0x02, 0xDA, 0xBC, 0x19, 0x10, 0xF0, 0x05},
    {0x0D, 0x78, 0x00, 0x71, 0x02, 0x88, 0x49, 0xA1, 0x30, 0x20, 0x15, 0x06, 0x08, 0x56, 0x3D}};

static const MfClassicBlock gen2_poller_default_block_0 = {
    .data =
        {0x00,
         0x01,
         0x02,
         0x03,
         0x00, // BCC - IMPORTANT
         0x08, // SAK
         0x04, // ATQA0
         0x00, // ATQA1
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00},
};

static const MfClassicBlock gen2_poller_default_empty_block = {
    .data =
        {0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00},
};

static MfClassicBlock gen2_poller_default_sector_trailer_block = {
    .data =
        {0xFF,
         0xFF,
         0xFF,
         0xFF,
         0xFF,
         0xFF,
         0xFF,
         0x07,
         0x80,
         0x69,
         0xFF,
         0xFF,
         0xFF,
         0xFF,
         0xFF,
         0xFF},
};

const char* const gen2_problem_strings[] = {
    "UID may be non-\nrewritable. Check data after writing",
    "No data in selected file",
    "Some sectors are locked",
    "Can't find keys to some sectors",
    "The selected file is incomplete",
};

Gen2Poller* gen2_poller_alloc(Nfc* nfc) {
    Gen2Poller* instance = malloc(sizeof(Gen2Poller));
    instance->poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a);
    instance->data = mf_classic_alloc();
    instance->crypto = crypto1_alloc();
    instance->tx_plain_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE);
    instance->tx_encrypted_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE);
    instance->rx_plain_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE);
    instance->rx_encrypted_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE);
    instance->card_state = Gen2CardStateLost;

    instance->gen2_event.data = &instance->gen2_event_data;

    instance->mode_ctx.write_ctx.mfc_data_source = malloc(sizeof(MfClassicData));
    instance->mode_ctx.write_ctx.mfc_data_target = malloc(sizeof(MfClassicData));

    instance->mode_ctx.write_ctx.need_halt_before_write = true;

    return instance;
}

void gen2_poller_free(Gen2Poller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);
    furi_assert(instance->crypto);
    furi_assert(instance->tx_plain_buffer);
    furi_assert(instance->rx_plain_buffer);
    furi_assert(instance->tx_encrypted_buffer);
    furi_assert(instance->rx_encrypted_buffer);

    nfc_poller_free(instance->poller);
    mf_classic_free(instance->data);
    crypto1_free(instance->crypto);
    bit_buffer_free(instance->tx_plain_buffer);
    bit_buffer_free(instance->rx_plain_buffer);
    bit_buffer_free(instance->tx_encrypted_buffer);
    bit_buffer_free(instance->rx_encrypted_buffer);

    free(instance->mode_ctx.write_ctx.mfc_data_source);
    free(instance->mode_ctx.write_ctx.mfc_data_target);

    free(instance);
}

NfcCommand gen2_poller_detect_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.event_data);
    furi_assert(event.instance);

    NfcCommand command = NfcCommandStop;
    Gen2PollerDetectContext* detect_ctx = context;
    Iso14443_3aPoller* iso3_poller = event.instance;
    Iso14443_3aPollerEvent* iso3_event = event.event_data;
    detect_ctx->error = Gen2PollerErrorTimeout;

    bit_buffer_reset(detect_ctx->tx_buffer);
    bit_buffer_append_byte(detect_ctx->tx_buffer, GEN2_CMD_READ_ATS);
    bit_buffer_append_byte(detect_ctx->tx_buffer, GEN2_FSDI_256 << 4);

    if(iso3_event->type == Iso14443_3aPollerEventTypeReady) {
        do {
            const Iso14443_3aError iso14443_3a_error = iso14443_3a_poller_send_standard_frame(
                iso3_poller, detect_ctx->tx_buffer, detect_ctx->rx_buffer, GEN2_POLLER_MAX_FWT);

            if(iso14443_3a_error != Iso14443_3aErrorNone &&
               iso14443_3a_error != Iso14443_3aErrorWrongCrc) {
                FURI_LOG_E(TAG, "ATS request failed");
                detect_ctx->error = Gen2PollerErrorProtocol;
                break;

            } else {
                FURI_LOG_D(TAG, "ATS request succeeded:");
                // Check against known ATS responses
                for(size_t i = 0; i < COUNT_OF(GEN2_ATS); i++) {
                    if(memcmp(
                           bit_buffer_get_data(detect_ctx->rx_buffer),
                           GEN2_ATS[i],
                           sizeof(GEN2_ATS[i])) == 0) {
                        detect_ctx->error = Gen2PollerErrorNone;
                        break;
                    }
                }
            }
        } while(false);
    } else if(iso3_event->type == Iso14443_3aPollerEventTypeError) {
        detect_ctx->error = Gen2PollerErrorTimeout;
    }
    furi_thread_flags_set(detect_ctx->thread_id, GEN2_POLLER_THREAD_FLAG_DETECTED);

    return command;
}

Gen2PollerError gen2_poller_detect(Nfc* nfc) {
    furi_assert(nfc);

    Gen2PollerDetectContext detect_ctx = {
        .poller = nfc_poller_alloc(nfc, NfcProtocolIso14443_3a),
        .tx_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE),
        .rx_buffer = bit_buffer_alloc(GEN2_POLLER_MAX_BUFFER_SIZE),
        .thread_id = furi_thread_get_current_id(),
        .detected = false,
        .error = Gen2PollerErrorNone,
    };

    nfc_poller_start(detect_ctx.poller, gen2_poller_detect_callback, &detect_ctx);
    uint32_t flags =
        furi_thread_flags_wait(GEN2_POLLER_THREAD_FLAG_DETECTED, FuriFlagWaitAny, FuriWaitForever);
    if(flags & GEN2_POLLER_THREAD_FLAG_DETECTED) {
        furi_thread_flags_clear(GEN2_POLLER_THREAD_FLAG_DETECTED);
    }
    nfc_poller_stop(detect_ctx.poller);

    bit_buffer_free(detect_ctx.tx_buffer);
    bit_buffer_free(detect_ctx.rx_buffer);
    nfc_poller_free(detect_ctx.poller);

    return detect_ctx.error;
}

NfcCommand gen2_poller_idle_handler(Gen2Poller* instance) {
    furi_assert(instance);

    NfcCommand command = NfcCommandContinue;

    instance->mode_ctx.write_ctx.current_block = 0;
    instance->gen2_event.type = Gen2PollerEventTypeDetected;
    command = instance->callback(instance->gen2_event, instance->context);
    instance->state = Gen2PollerStateRequestMode;

    return command;
}

NfcCommand gen2_poller_request_mode_handler(Gen2Poller* instance) {
    furi_assert(instance);

    NfcCommand command = NfcCommandContinue;

    instance->gen2_event.type = Gen2PollerEventTypeRequestMode;
    command = instance->callback(instance->gen2_event, instance->context);
    instance->mode = instance->gen2_event_data.poller_mode.mode;
    if(instance->gen2_event_data.poller_mode.mode == Gen2PollerModeWipe) {
        instance->state = Gen2PollerStateWriteTargetDataRequest;
    } else {
        instance->state = Gen2PollerStateWriteSourceDataRequest;
    }

    return command;
}

NfcCommand gen2_poller_write_source_data_request_handler(Gen2Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen2_event.type = Gen2PollerEventTypeRequestDataToWrite;
    command = instance->callback(instance->gen2_event, instance->context);
    memcpy(
        instance->mode_ctx.write_ctx.mfc_data_source,
        instance->gen2_event_data.data_to_write.mfc_data,
        sizeof(MfClassicData));
    instance->state = Gen2PollerStateWriteTargetDataRequest;

    return command;
}

NfcCommand gen2_poller_write_target_data_request_handler(Gen2Poller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen2_event.type = Gen2PollerEventTypeRequestTargetData;
    command = instance->callback(instance->gen2_event, instance->context);
    memcpy(
        instance->mode_ctx.write_ctx.mfc_data_target,
        instance->gen2_event_data.target_data.mfc_data,
        sizeof(MfClassicData));
    if(instance->mode == Gen2PollerModeWipe) {
        instance->state = Gen2PollerStateWipe;
    } else {
        instance->state = Gen2PollerStateWrite;
    }

    return command;
}

Gen2PollerError gen2_poller_write_block_handler(
    Gen2Poller* instance,
    uint8_t block_num,
    const MfClassicBlock* block) {
    furi_assert(instance);

    Gen2PollerError error = Gen2PollerErrorNone;
    Gen2PollerWriteContext* write_ctx = &instance->mode_ctx.write_ctx;
    MfClassicKey auth_key = write_ctx->auth_key;

    do {
        // Compare the target and source data
        if(memcmp(block->data, write_ctx->mfc_data_target->block[block_num].data, 16) == 0) {
            FURI_LOG_D(TAG, "Block %d is the same, skipping", block_num);
            break;
        }

        // Reauth if necessary
        if(write_ctx->need_halt_before_write) {
            FURI_LOG_D(TAG, "Auth before writing block %d", write_ctx->current_block);
            error = gen2_poller_auth(
                instance, write_ctx->current_block, &auth_key, write_ctx->write_key, NULL);
            if(error != Gen2PollerErrorNone) {
                FURI_LOG_D(
                    TAG, "Failed to auth to block %d for writing", write_ctx->current_block);
                break;
            }
        }

        // Write the block
        error = gen2_poller_write_block(instance, write_ctx->current_block, block);
        if(error != Gen2PollerErrorNone) {
            FURI_LOG_D(TAG, "Failed to write block %d", write_ctx->current_block);
            break;
        }
    } while(false);
    FURI_LOG_D(TAG, "Block %d finished, halting", write_ctx->current_block);
    gen2_poller_halt(instance);
    return error;
}

NfcCommand gen2_poller_wipe_handler(Gen2Poller* instance) {
    NfcCommand command = NfcCommandContinue;
    Gen2PollerError error = Gen2PollerErrorNone;
    Gen2PollerWriteContext* write_ctx = &instance->mode_ctx.write_ctx;
    uint8_t block_num = write_ctx->current_block;

    do {
        // Check whether the ACs for that block are known in target data
        if(!mf_classic_is_block_read(
               write_ctx->mfc_data_target,
               mf_classic_get_sector_trailer_num_by_block(block_num))) {
            FURI_LOG_E(TAG, "Sector trailer for block %d not present in target data", block_num);
            break;
        }

        // Check whether ACs need to be reset and whether they can be reset
        if(!gen2_poller_can_write_block(write_ctx->mfc_data_target, block_num)) {
            if(!gen2_can_reset_access_conditions(write_ctx->mfc_data_target, block_num)) {
                FURI_LOG_E(TAG, "Block %d cannot be written", block_num);
                break;
            } else {
                FURI_LOG_D(TAG, "Resetting ACs for block %d", block_num);
                // Generate a block with old keys and default ACs (0xFF, 0x07, 0x80)
                MfClassicBlock block;
                memset(&block, 0, sizeof(block));
                memcpy(block.data, write_ctx->mfc_data_target->block[block_num].data, 16);
                memcpy(block.data + 6, "\xFF\x07\x80", 3);

                error = gen2_poller_write_block_handler(instance, block_num, &block);
                if(error != Gen2PollerErrorNone) {
                    FURI_LOG_E(TAG, "Failed to reset ACs for block %d", block_num);
                    break;
                } else {
                    FURI_LOG_D(TAG, "ACs for block %d reset", block_num);
                    memcpy(write_ctx->mfc_data_target->block[block_num].data, block.data, 16);
                }
            }
        }

        // Figure out which key to use for writing
        write_ctx->write_key =
            gen2_poller_get_key_type_to_write(write_ctx->mfc_data_target, block_num);

        // Get the key to use for writing from the target data
        MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(
            write_ctx->mfc_data_target, mf_classic_get_sector_by_block(block_num));
        if(write_ctx->write_key == MfClassicKeyTypeA) {
            write_ctx->auth_key = sec_tr->key_a;
        } else {
            write_ctx->auth_key = sec_tr->key_b;
        }

        // Write the default block depending on the block type
        if(block_num == 0) {
            error =
                gen2_poller_write_block_handler(instance, block_num, &gen2_poller_default_block_0);
        } else if(mf_classic_is_sector_trailer(block_num)) {
            error = gen2_poller_write_block_handler(
                instance, block_num, &gen2_poller_default_sector_trailer_block);
        } else {
            error = gen2_poller_write_block_handler(
                instance, block_num, &gen2_poller_default_empty_block);
        }
        if(error != Gen2PollerErrorNone) {
            FURI_LOG_E(TAG, "Couldn't write block %d", block_num);
        }
    } while(false);

    write_ctx->current_block++;

    if(error != Gen2PollerErrorNone) {
        FURI_LOG_D(TAG, "Error occurred: %d", error);
    }

    if(write_ctx->current_block ==
       mf_classic_get_total_block_num(write_ctx->mfc_data_target->type)) {
        instance->state = Gen2PollerStateSuccess;
    }

    return command;
}

NfcCommand gen2_poller_write_handler(Gen2Poller* instance) {
    NfcCommand command = NfcCommandContinue;
    Gen2PollerError error = Gen2PollerErrorNone;
    Gen2PollerWriteContext* write_ctx = &instance->mode_ctx.write_ctx;
    uint8_t block_num = write_ctx->current_block;

    do {
        // Check whether the block is present in the source data
        if(!mf_classic_is_block_read(write_ctx->mfc_data_source, block_num)) {
            // FURI_LOG_E(TAG, "Block %d not present in source data", block_num);
            break;
        }

        // Check whether the ACs for that block are known in target data
        if(!mf_classic_is_block_read(
               write_ctx->mfc_data_target,
               mf_classic_get_sector_trailer_num_by_block(block_num))) {
            FURI_LOG_E(TAG, "Sector trailer for block %d not present in target data", block_num);
            break;
        }

        // Check whether ACs need to be reset and whether they can be reset
        if(!gen2_poller_can_write_block(write_ctx->mfc_data_target, block_num)) {
            if(!gen2_can_reset_access_conditions(write_ctx->mfc_data_target, block_num)) {
                FURI_LOG_E(TAG, "Block %d cannot be written", block_num);
                break;
            } else {
                FURI_LOG_D(TAG, "Resetting ACs for block %d", block_num);
                // Generate a block with old keys and default ACs (0xFF, 0x07, 0x80)
                MfClassicBlock block;
                memset(&block, 0, sizeof(block));
                memcpy(block.data, write_ctx->mfc_data_target->block[block_num].data, 16);
                memcpy(block.data + 6, "\xFF\x07\x80", 3);

                error = gen2_poller_write_block_handler(instance, block_num, &block);
                if(error != Gen2PollerErrorNone) {
                    FURI_LOG_E(TAG, "Failed to reset ACs for block %d", block_num);
                    break;
                } else {
                    FURI_LOG_D(TAG, "ACs for block %d reset", block_num);
                    memcpy(write_ctx->mfc_data_target->block[block_num].data, block.data, 16);
                }
            }
        }

        // Figure out which key to use for writing
        write_ctx->write_key =
            gen2_poller_get_key_type_to_write(write_ctx->mfc_data_target, block_num);

        // Get the key to use for writing from the target data
        MfClassicSectorTrailer* sec_tr = mf_classic_get_sector_trailer_by_sector(
            write_ctx->mfc_data_target, mf_classic_get_sector_by_block(block_num));
        if(write_ctx->write_key == MfClassicKeyTypeA) {
            write_ctx->auth_key = sec_tr->key_a;
        } else {
            write_ctx->auth_key = sec_tr->key_b;
        }

        // Write the block
        error = gen2_poller_write_block_handler(
            instance, block_num, &write_ctx->mfc_data_source->block[block_num]);
        if(error != Gen2PollerErrorNone) {
            FURI_LOG_E(TAG, "Couldn't write block %d", block_num);
        }
    } while(false);
    write_ctx->current_block++;

    if(error != Gen2PollerErrorNone) {
        FURI_LOG_D(TAG, "Error occurred: %d", error);
    } else if(
        write_ctx->current_block ==
        mf_classic_get_total_block_num(write_ctx->mfc_data_source->type)) {
        instance->state = Gen2PollerStateSuccess;
    }

    return command;
}

NfcCommand gen2_poller_success_handler(Gen2Poller* instance) {
    furi_assert(instance);

    NfcCommand command = NfcCommandContinue;

    instance->gen2_event.type = Gen2PollerEventTypeSuccess;
    command = instance->callback(instance->gen2_event, instance->context);
    instance->state = Gen2PollerStateIdle;

    return command;
}

NfcCommand gen2_poller_fail_handler(Gen2Poller* instance) {
    furi_assert(instance);

    NfcCommand command = NfcCommandContinue;

    instance->gen2_event.type = Gen2PollerEventTypeFail;
    command = instance->callback(instance->gen2_event, instance->context);
    instance->state = Gen2PollerStateIdle;

    return command;
}

static const Gen2PollerStateHandler gen2_poller_state_handlers[Gen2PollerStateNum] = {
    [Gen2PollerStateIdle] = gen2_poller_idle_handler,
    [Gen2PollerStateRequestMode] = gen2_poller_request_mode_handler,
    [Gen2PollerStateWipe] = gen2_poller_wipe_handler,
    [Gen2PollerStateWriteSourceDataRequest] = gen2_poller_write_source_data_request_handler,
    [Gen2PollerStateWriteTargetDataRequest] = gen2_poller_write_target_data_request_handler,
    [Gen2PollerStateWrite] = gen2_poller_write_handler,
    [Gen2PollerStateSuccess] = gen2_poller_success_handler,
    [Gen2PollerStateFail] = gen2_poller_fail_handler,
};

NfcCommand gen2_poller_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_3a);
    furi_assert(event.event_data);
    furi_assert(event.instance);

    NfcCommand command = NfcCommandContinue;
    Gen2Poller* instance = context;
    instance->iso3_poller = event.instance;
    Iso14443_3aPollerEvent* iso3_event = event.event_data;

    if(iso3_event->type == Iso14443_3aPollerEventTypeReady) {
        command = gen2_poller_state_handlers[instance->state](instance);
    }

    return command;
}

void gen2_poller_start(Gen2Poller* instance, Gen2PollerCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    nfc_poller_start(instance->poller, gen2_poller_callback, instance);
    return;
}

void gen2_poller_stop(Gen2Poller* instance) {
    furi_assert(instance);

    FURI_LOG_D(TAG, "Stopping Gen2 poller");
    nfc_poller_stop(instance->poller);
    return;
}

Gen2PollerWriteProblems gen2_poller_check_target_problems(NfcDevice* target_dev) {
    furi_assert(target_dev);

    Gen2PollerWriteProblems problems = {0};
    const MfClassicData* mfc_data = nfc_device_get_data(target_dev, NfcProtocolMfClassic);

    if(mfc_data) {
        uint16_t total_block_num = mf_classic_get_total_block_num(mfc_data->type);
        for(uint16_t i = 0; i < total_block_num; i++) {
            if(mf_classic_is_sector_trailer(i)) {
                problems.all_problems |=
                    gen2_poller_can_write_sector_trailer(mfc_data, i).all_problems;
            } else {
                problems.all_problems |=
                    gen2_poller_can_write_data_block(mfc_data, i).all_problems;
            }
        }
    } else {
        problems.no_data = true;
    }

    return problems;
}

Gen2PollerWriteProblems gen2_poller_check_source_problems(NfcDevice* source_dev) {
    furi_assert(source_dev);

    Gen2PollerWriteProblems problems = {0};
    const MfClassicData* mfc_data = nfc_device_get_data(source_dev, NfcProtocolMfClassic);

    if(mfc_data) {
        uint16_t total_block_num = mf_classic_get_total_block_num(mfc_data->type);
        for(uint16_t i = 0; i < total_block_num; i++) {
            if(!mf_classic_is_block_read(mfc_data, i)) {
                problems.missing_source_data = true;
            }
        }
    }

    return problems;
}
