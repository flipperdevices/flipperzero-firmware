#include "legic_prime_poller_sync.h"
#include "legic_prime_poller_i.h"

#include <nfc/nfc_poller.h>
#include <furi.h>

#define LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE (1UL << 0)

typedef struct {
    FuriThreadId thread_id;
    LegicPrimeError error;
    LegicPrimeData* data;
    // Write mode when write_data is set.
    uint16_t write_offset;
    const uint8_t* write_data;
    uint16_t write_size;
} LegicPrimePollerSyncContext;

static NfcCommand legic_prime_poller_sync_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolLegicPrime);

    LegicPrimePollerSyncContext* ctx = context;
    LegicPrimePollerEvent* legic_event = event.event_data;

    NfcCommand command = NfcCommandContinue;
    if(legic_event->type == LegicPrimePollerEventTypeRequestMode) {
        LegicPrimePollerEventDataModeRequest* request = &legic_event->data->mode_request;
        if(ctx->write_data) {
            request->mode = LegicPrimePollerModeWrite;
            request->write_offset = ctx->write_offset;
            request->write_data = ctx->write_data;
            request->write_size = ctx->write_size;
        } else {
            request->mode = LegicPrimePollerModeRead;
        }
    } else if(legic_event->type == LegicPrimePollerEventTypeSuccess) {
        if(ctx->data) {
            memcpy(ctx->data, legic_prime_poller_get_data(event.instance), sizeof(LegicPrimeData));
        }
        ctx->error = LegicPrimeErrorNone;
        furi_thread_flags_set(ctx->thread_id, LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE);
        command = NfcCommandStop;
    } else if(legic_event->type == LegicPrimePollerEventTypeFailure) {
        ctx->error = legic_event->data->error;
        furi_thread_flags_set(ctx->thread_id, LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE);
        command = NfcCommandStop;
    }
    return command;
}

LegicPrimeError legic_prime_poller_sync_read(Nfc* nfc, LegicPrimeData* data) {
    furi_check(nfc);
    furi_check(data);

    LegicPrimePollerSyncContext ctx = {
        .thread_id = furi_thread_get_current_id(),
        .error = LegicPrimeErrorNotPresent,
        .data = data,
    };

    NfcPoller* poller = nfc_poller_alloc(nfc, NfcProtocolLegicPrime);
    nfc_poller_start(poller, legic_prime_poller_sync_callback, &ctx);
    furi_thread_flags_wait(
        LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE);

    nfc_poller_stop(poller);
    nfc_poller_free(poller);

    return ctx.error;
}

LegicPrimeError
    legic_prime_poller_sync_write(Nfc* nfc, uint16_t offset, const uint8_t* data, size_t size) {
    furi_check(nfc);
    furi_check(data);
    furi_check(size > 0 && size <= LEGIC_PRIME_MEM_MAX);

    LegicPrimePollerSyncContext ctx = {
        .thread_id = furi_thread_get_current_id(),
        .error = LegicPrimeErrorNotPresent,
        .write_offset = offset,
        .write_data = data,
        .write_size = size,
    };

    NfcPoller* poller = nfc_poller_alloc(nfc, NfcProtocolLegicPrime);
    nfc_poller_start(poller, legic_prime_poller_sync_callback, &ctx);
    furi_thread_flags_wait(
        LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE, FuriFlagWaitAny, FuriWaitForever);
    furi_thread_flags_clear(LEGIC_PRIME_POLLER_FLAG_COMMAND_COMPLETE);

    nfc_poller_stop(poller);
    nfc_poller_free(poller);

    return ctx.error;
}
