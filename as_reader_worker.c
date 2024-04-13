#include "as_reader_worker.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller.h>

#define TAG "AsReaderWorker"
#define WORKERSTOP (1UL << 0UL)
#define POLLERFINISH (1UL << 1UL)
#define POLLERTRXFINISH (1UL << 2UL)

void as_reader_worker_change_state(AsReaderWorker* as_reader_worker, AsReaderWorkerState state) {
    as_reader_worker->state = state;
    if(as_reader_worker->callback) as_reader_worker->callback(state, as_reader_worker->nfc_relay);
}

AsReaderWorker* as_reader_worker_alloc(NfcRelay* nfc_relay) {
    furi_assert(nfc_relay);
    FURI_LOG_D(TAG, "as_reader_worker_alloc");

    AsReaderWorker* as_reader_worker = malloc(sizeof(AsReaderWorker));
    as_reader_worker->thread = furi_thread_alloc_ex(
        "AsReaderWorkerThread", 2048, as_reader_worker_task, as_reader_worker);
    as_reader_worker->running = false;
    as_reader_worker->comm = comm_alloc(nfc_relay->config);
    as_reader_worker->nfc = nfc_alloc();
    as_reader_worker->bitbuffer_rx = bit_buffer_alloc(256);
    as_reader_worker->bitbuffer_tx = bit_buffer_alloc(256);
    as_reader_worker->nfc_relay = nfc_relay;
    as_reader_worker->callback = NULL;
    return as_reader_worker;
}

void as_reader_worker_free(AsReaderWorker* as_reader_worker) {
    furi_assert(as_reader_worker);
    FURI_LOG_D(TAG, "as_reader_worker_free");

    comm_free(as_reader_worker->comm);
    nfc_free(as_reader_worker->nfc);
    bit_buffer_free(as_reader_worker->bitbuffer_rx);
    bit_buffer_free(as_reader_worker->bitbuffer_tx);
    furi_thread_free(as_reader_worker->thread);
    free(as_reader_worker);
}

void as_reader_worker_stop(AsReaderWorker* as_reader_worker) {
    furi_assert(as_reader_worker);
    FURI_LOG_D(TAG, "as_reader_worker_stop");

    as_reader_worker->running = false;
    furi_thread_flags_set(furi_thread_get_id(as_reader_worker->thread), WORKERSTOP);
    furi_thread_join(as_reader_worker->thread);
    comm_deinit(as_reader_worker->comm);
    as_reader_worker->callback = NULL;
}

void as_reader_worker_start(AsReaderWorker* as_reader_worker, AsReaderWorkerCallback callback) {
    FURI_LOG_D(TAG, "as_reader_worker_start");
    furi_assert(as_reader_worker);
    as_reader_worker->running = true;

    comm_init(as_reader_worker->comm);
    as_reader_worker->callback = callback;
    furi_thread_start(as_reader_worker->thread);
}

static NfcCommand as_reader_worker_poller_trx_callback(NfcGenericEvent event, void* context) {
    furi_assert(event.protocol == NfcProtocolIso14443_4a);
    furi_assert(context);

    AsReaderWorker* as_reader_worker = context;
    const Iso14443_4aPollerEvent* iso14443_4a_event = event.event_data;

    if(iso14443_4a_event->type == Iso14443_4aPollerEventTypeReady) {
        if(as_reader_worker->state == AsReaderWorkerStateCardSearch) {
            //FURI_LOG_D(TAG, "Card Found");
            as_reader_worker_change_state(as_reader_worker, AsReaderWorkerStateCardFound);
            furi_thread_flags_set(furi_thread_get_id(as_reader_worker->thread), POLLERFINISH);
            return NfcCommandContinue;
        } else if(as_reader_worker->state == AsReaderWorkerStateInteractive) {
            if(as_reader_worker->apdu_ready) {
                FURI_LOG_D(TAG, "Card Found, Send TX");
                Iso14443_4aError err = iso14443_4a_poller_send_block(
                    event.instance,
                    as_reader_worker->bitbuffer_tx,
                    as_reader_worker->bitbuffer_rx);
                if(err == Iso14443_4aErrorNone) {
                    FURI_LOG_D(TAG, "Card Found, Recv RX");
                    NfcRelayPacket* packet = packet_alloc_data(
                        NfcRelayPacketApduResp,
                        bit_buffer_get_size_bytes(as_reader_worker->bitbuffer_rx),
                        bit_buffer_get_data(as_reader_worker->bitbuffer_rx));
                    comm_write_packet(as_reader_worker->comm, packet);
                    free(packet);
                    FURI_LOG_D(TAG, "Card Found, Sent Relay Packet");
                } else {
                    FURI_LOG_E(TAG, "NfcError: %d", err);
                }
                as_reader_worker->apdu_ready = false;
                furi_thread_flags_set(
                    furi_thread_get_id(as_reader_worker->thread), POLLERTRXFINISH);
                return NfcCommandContinue;
            } else {
                //FURI_LOG_T(TAG, "Card Ready, but apdu data not");
                furi_delay_ms(1);
                return NfcCommandContinue;
            }
        } else if(as_reader_worker->state == AsReaderWorkerStateCardFound) {
            FURI_LOG_D(TAG, "still in AsReaderWorkerStateCardFound");
            furi_delay_ms(1000);
            return NfcCommandContinue;
        } else {
            FURI_LOG_D(TAG, "WHAT?");
            return NfcCommandStop;
        }
    }
    furi_thread_flags_set(furi_thread_get_id(as_reader_worker->thread), POLLERTRXFINISH);
    return NfcCommandContinue;
}

int32_t as_reader_worker_task(void* context) {
    furi_assert(context);
    AsReaderWorker* as_reader_worker = context;

    FURI_LOG_D(TAG, "Send Ping Packet");
    comm_send_pingpong(as_reader_worker->comm, NfcRelayPacketPing, NfcRelayAsReader);
    NfcRelayPacket* packet = NULL;

    while(1) {
        if(!as_reader_worker->running) {
            FURI_LOG_D(TAG, "as_reader_worker stop running, break");
            break;
        }
        if(as_reader_worker->state == AsReaderWorkerStateWaitPong) {
            if(comm_wait_pong(as_reader_worker->comm, NfcRelayAsCard, NfcRelayAsReader)) {
                FURI_LOG_D(TAG, "comm_wait_pong succ, change state");
                as_reader_worker_change_state(as_reader_worker, AsReaderWorkerStateCardSearch);
                as_reader_worker->poller =
                    nfc_poller_alloc(as_reader_worker->nfc, NfcProtocolIso14443_4a);
                nfc_poller_start(
                    as_reader_worker->poller,
                    as_reader_worker_poller_trx_callback,
                    as_reader_worker);
            } else {
                furi_delay_ms(100);
                continue;
            }
        } else if(as_reader_worker->state == AsReaderWorkerStateCardSearch) {
            FURI_LOG_D(TAG, "AsReaderWorkerStateCardSearch");
            furi_thread_flags_wait(POLLERFINISH || WORKERSTOP, FuriFlagWaitAny, 1000);
            continue;
        } else if(as_reader_worker->state == AsReaderWorkerStateCardFound) {
            FURI_LOG_D(TAG, "Card Found");
            const Iso14443_4aData* dev_data = nfc_poller_get_data(as_reader_worker->poller);
            SerializedIso14443_4a* serialized = iso14443_4a_serialize(dev_data);
            packet = packet_alloc_data(
                NfcRelayPacketNfcDevData, sizeof(*serialized) + serialized->len_t1_tk, serialized);
            FURI_LOG_D(TAG, "packet->len: %d", packet->len);
            comm_write_packet(as_reader_worker->comm, packet);
            free(packet);
            packet = NULL;
            free(serialized);
            serialized = NULL;
            as_reader_worker_change_state(as_reader_worker, AsReaderWorkerStateInteractive);
            continue;
        } else if(as_reader_worker->state == AsReaderWorkerStateInteractive) {
            NfcRelayPacket* recv_packet;
            recv_packet = comm_wait_packet(as_reader_worker->comm, NfcRelayPacketApduReq);
            if(!recv_packet) {
                continue;
            }
            FURI_LOG_D(TAG, "Recv NfcRelayPacketApduReq");
            // drop PCB
            bit_buffer_copy_bytes(
                as_reader_worker->bitbuffer_tx, &recv_packet->buf[1], recv_packet->len - 1);
#ifdef FURI_DEBUG
            FuriString* debug_buf;
            debug_buf = furi_string_alloc();
            for(size_t i = 0; i < bit_buffer_get_size_bytes(as_reader_worker->bitbuffer_tx); i++) {
                furi_string_cat_printf(
                    debug_buf, " %02X", bit_buffer_get_byte(as_reader_worker->bitbuffer_tx, i));
            }
            furi_string_trim(debug_buf);
            FURI_LOG_T(
                TAG,
                "TX (%d): %s",
                bit_buffer_get_size(as_reader_worker->bitbuffer_tx),
                furi_string_get_cstr(debug_buf));
            furi_string_free(debug_buf);
#endif
            free(recv_packet);
            recv_packet = NULL;
            as_reader_worker->apdu_ready = true;
            furi_thread_flags_wait(POLLERTRXFINISH || WORKERSTOP, FuriFlagWaitAny, 100);
        }
    }
    if(as_reader_worker->poller) {
        nfc_poller_stop(as_reader_worker->poller);
        nfc_poller_free(as_reader_worker->poller);
        as_reader_worker->poller = NULL;
    }
    FURI_LOG_D(TAG, "as_reader_worker stop running, poller freed");

    return 0;
}