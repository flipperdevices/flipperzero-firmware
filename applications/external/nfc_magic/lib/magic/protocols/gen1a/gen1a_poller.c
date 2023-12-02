#include "gen1a_poller_i.h"
#include <nfc/protocols/iso14443_3a/iso14443_3a.h>
#include <nfc/protocols/iso14443_3a/iso14443_3a_poller.h>
#include <nfc/helpers/nfc_data_generator.h>

#include <furi/furi.h>

#define GEN1A_POLLER_THREAD_FLAG_DETECTED (1U << 0)

typedef NfcCommand (*Gen1aPollerStateHandler)(Gen1aPoller* instance);

typedef struct {
    Nfc* nfc;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    FuriThreadId thread_id;
    bool detected;
} Gen1aPollerDetectContext;

Gen1aPoller* gen1a_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    Gen1aPoller* instance = malloc(sizeof(Gen1aPoller));
    instance->nfc = nfc;

    nfc_config(instance->nfc, NfcModePoller, NfcTechIso14443a);
    nfc_set_guard_time_us(instance->nfc, ISO14443_3A_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, ISO14443_3A_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, ISO14443_3A_POLL_POLL_MIN_US);

    instance->tx_buffer = bit_buffer_alloc(GEN1A_POLLER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(GEN1A_POLLER_MAX_BUFFER_SIZE);

    instance->mfc_device = nfc_device_alloc();

    instance->gen1a_event.data = &instance->gen1a_event_data;

    return instance;
}

void gen1a_poller_free(Gen1aPoller* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);

    nfc_device_free(instance->mfc_device);

    free(instance);
}

NfcCommand gen1a_poller_detect_callback(NfcEvent event, void* context) {
    furi_assert(context);

    NfcCommand command = NfcCommandStop;
    Gen1aPollerDetectContext* gen1a_poller_detect_ctx = context;

    if(event.type == NfcEventTypePollerReady) {
        do {
            bit_buffer_set_size(gen1a_poller_detect_ctx->tx_buffer, 7);
            bit_buffer_set_byte(gen1a_poller_detect_ctx->tx_buffer, 0, 0x40);

            NfcError error = nfc_poller_trx(
                gen1a_poller_detect_ctx->nfc,
                gen1a_poller_detect_ctx->tx_buffer,
                gen1a_poller_detect_ctx->rx_buffer,
                GEN1A_POLLER_MAX_FWT);

            if(error != NfcErrorNone) break;
            if(bit_buffer_get_size(gen1a_poller_detect_ctx->rx_buffer) != 4) break;
            if(bit_buffer_get_byte(gen1a_poller_detect_ctx->rx_buffer, 0) != 0x0A) break;

            gen1a_poller_detect_ctx->detected = true;
        } while(false);
    }
    furi_thread_flags_set(gen1a_poller_detect_ctx->thread_id, GEN1A_POLLER_THREAD_FLAG_DETECTED);

    return command;
}

bool gen1a_poller_detect(Nfc* nfc) {
    furi_assert(nfc);

    nfc_config(nfc, NfcModePoller, NfcTechIso14443a);
    nfc_set_guard_time_us(nfc, ISO14443_3A_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(nfc, ISO14443_3A_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(nfc, ISO14443_3A_POLL_POLL_MIN_US);

    Gen1aPollerDetectContext gen1a_poller_detect_ctx = {};
    gen1a_poller_detect_ctx.nfc = nfc;
    gen1a_poller_detect_ctx.tx_buffer = bit_buffer_alloc(GEN1A_POLLER_MAX_BUFFER_SIZE);
    gen1a_poller_detect_ctx.rx_buffer = bit_buffer_alloc(GEN1A_POLLER_MAX_BUFFER_SIZE);
    gen1a_poller_detect_ctx.thread_id = furi_thread_get_current_id();
    gen1a_poller_detect_ctx.detected = false;

    nfc_start(nfc, gen1a_poller_detect_callback, &gen1a_poller_detect_ctx);
    uint32_t flags = furi_thread_flags_wait(
        GEN1A_POLLER_THREAD_FLAG_DETECTED, FuriFlagWaitAny, FuriWaitForever);
    if(flags & GEN1A_POLLER_THREAD_FLAG_DETECTED) {
        furi_thread_flags_clear(GEN1A_POLLER_THREAD_FLAG_DETECTED);
    }
    nfc_stop(nfc);

    bit_buffer_free(gen1a_poller_detect_ctx.tx_buffer);
    bit_buffer_free(gen1a_poller_detect_ctx.rx_buffer);

    return gen1a_poller_detect_ctx.detected;
}

static void gen1a_poller_reset(Gen1aPoller* instance) {
    instance->current_block = 0;
    nfc_data_generator_fill_data(NfcDataGeneratorTypeMfClassic1k_4b, instance->mfc_device);
}

NfcCommand gen1a_poller_idle_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    gen1a_poller_reset(instance);
    Gen1aPollerError error = gen1a_poller_wupa(instance);
    if(error == Gen1aPollerErrorNone) {
        instance->state = Gen1aPollerStateRequestMode;
        instance->gen1a_event.type = Gen1aPollerEventTypeDetected;
        command = instance->callback(instance->gen1a_event, instance->context);
    }

    return command;
}

NfcCommand gen1a_poller_request_mode_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen1a_event.type = Gen1aPollerEventTypeRequestMode;
    command = instance->callback(instance->gen1a_event, instance->context);
    if(instance->gen1a_event_data.request_mode.mode == Gen1aPollerModeWipe) {
        instance->state = Gen1aPollerStateWipe;
    } else {
        instance->state = Gen1aPollerStateWriteDataRequest;
    }

    return command;
}

NfcCommand gen1a_poller_wipe_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    Gen1aPollerError error = Gen1aPollerErrorNone;

    const MfClassicData* mfc_data =
        nfc_device_get_data(instance->mfc_device, NfcProtocolMfClassic);
    uint16_t total_block_num = mf_classic_get_total_block_num(mfc_data->type);

    if(instance->current_block == total_block_num) {
        instance->state = Gen1aPollerStateSuccess;
    } else {
        do {
            if(instance->current_block == 0) {
                error = gen1a_poller_data_access(instance);
                if(error != Gen1aPollerErrorNone) {
                    instance->state = Gen1aPollerStateFail;
                    break;
                }
            }
            error = gen1a_poller_write_block(
                instance, instance->current_block, &mfc_data->block[instance->current_block]);
            if(error != Gen1aPollerErrorNone) {
                instance->state = Gen1aPollerStateFail;
                break;
            }
            instance->current_block++;
        } while(false);
    }

    return command;
}

NfcCommand gen1a_poller_write_data_request_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen1a_event.type = Gen1aPollerEventTypeRequestDataToWrite;
    command = instance->callback(instance->gen1a_event, instance->context);
    instance->state = Gen1aPollerStateWrite;

    return command;
}

NfcCommand gen1a_poller_write_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;
    Gen1aPollerError error = Gen1aPollerErrorNone;

    const MfClassicData* mfc_data = instance->gen1a_event_data.data_to_write.mfc_data;
    uint16_t total_block_num = mf_classic_get_total_block_num(mfc_data->type);

    if(instance->current_block == total_block_num) {
        instance->state = Gen1aPollerStateSuccess;
    } else {
        do {
            if(instance->current_block == 0) {
                error = gen1a_poller_data_access(instance);
                if(error != Gen1aPollerErrorNone) {
                    instance->state = Gen1aPollerStateFail;
                    break;
                }
            }
            error = gen1a_poller_write_block(
                instance, instance->current_block, &mfc_data->block[instance->current_block]);
            if(error != Gen1aPollerErrorNone) {
                instance->state = Gen1aPollerStateFail;
                break;
            }
            instance->current_block++;
        } while(false);
    }

    return command;
}

NfcCommand gen1a_poller_success_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen1a_event.type = Gen1aPollerEventTypeSuccess;
    command = instance->callback(instance->gen1a_event, instance->context);
    instance->state = Gen1aPollerStateIdle;

    return command;
}

NfcCommand gen1a_poller_fail_handler(Gen1aPoller* instance) {
    NfcCommand command = NfcCommandContinue;

    instance->gen1a_event.type = Gen1aPollerEventTypeFail;
    command = instance->callback(instance->gen1a_event, instance->context);
    instance->state = Gen1aPollerStateIdle;

    return command;
}

static const Gen1aPollerStateHandler gen1a_poller_state_handlers[Gen1aPollerStateNum] = {
    [Gen1aPollerStateIdle] = gen1a_poller_idle_handler,
    [Gen1aPollerStateRequestMode] = gen1a_poller_request_mode_handler,
    [Gen1aPollerStateWipe] = gen1a_poller_wipe_handler,
    [Gen1aPollerStateWriteDataRequest] = gen1a_poller_write_data_request_handler,
    [Gen1aPollerStateWrite] = gen1a_poller_write_handler,
    [Gen1aPollerStateSuccess] = gen1a_poller_success_handler,
    [Gen1aPollerStateFail] = gen1a_poller_fail_handler,

};

NfcCommand gen1a_poller_run(NfcEvent event, void* context) {
    NfcCommand command = NfcCommandContinue;
    Gen1aPoller* instance = context;

    if(event.type == NfcEventTypePollerReady) {
        command = gen1a_poller_state_handlers[instance->state](instance);
    }

    if(instance->session_state == Gen1aPollerSessionStateStopRequest) {
        command = NfcCommandStop;
    }

    return command;
}

void gen1a_poller_start(Gen1aPoller* instance, Gen1aPollerCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;

    instance->session_state = Gen1aPollerSessionStateStarted;
    nfc_start(instance->nfc, gen1a_poller_run, instance);
}

void gen1a_poller_stop(Gen1aPoller* instance) {
    furi_assert(instance);

    instance->session_state = Gen1aPollerSessionStateStopRequest;
    nfc_stop(instance->nfc);
    instance->session_state = Gen1aPollerSessionStateIdle;
}
