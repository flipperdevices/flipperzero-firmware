#include "as_reader_worker.h"

#define TAG "AsReaderWorker"

void as_reader_worker_change_state(AsReaderWorker* as_reader_worker, AsReaderWorkerState state) {
    as_reader_worker->state = state;
    if(as_reader_worker->callback) as_reader_worker->callback(state, as_reader_worker->nfc_relay);
}

AsReaderWorker* as_reader_worker_alloc(NfcRelay* nfc_relay) {
    furi_assert(nfc_relay);
    FURI_LOG_D(TAG, "as_reader_worker_alloc");

    AsReaderWorker* as_reader_worker = malloc(sizeof(AsReaderWorker));
    as_reader_worker->thread = furi_thread_alloc_ex(
        "AsReaderWorkerThread", 8192, as_reader_worker_task, as_reader_worker);
    as_reader_worker->running = false;
    as_reader_worker->comm = comm_alloc(nfc_relay->config);
    as_reader_worker->nfc_relay = nfc_relay;
    as_reader_worker->callback = NULL;
    return as_reader_worker;
}

void as_reader_worker_free(AsReaderWorker* as_reader_worker) {
    furi_assert(as_reader_worker);

    comm_free(as_reader_worker->comm);
    furi_thread_free(as_reader_worker->thread);
    free(as_reader_worker);
}

void as_reader_worker_stop(AsReaderWorker* as_reader_worker) {
    furi_assert(as_reader_worker);

    as_reader_worker->running = false;
    comm_deinit(as_reader_worker->comm);
    as_reader_worker->callback = NULL;

    furi_thread_join(as_reader_worker->thread);
}

void as_reader_worker_start(AsReaderWorker* as_reader_worker, AsReaderWorkerCallback callback) {
    FURI_LOG_D(TAG, "as_reader_worker_start");
    furi_assert(as_reader_worker);
    as_reader_worker->running = true;

    comm_init(as_reader_worker->comm);
    as_reader_worker->callback = callback;
    furi_thread_start(as_reader_worker->thread);
}

int32_t as_reader_worker_task(void* context) {
    furi_assert(context);
    AsReaderWorker* as_reader_worker = context;

    furi_hal_nfc_deinit();
    furi_hal_nfc_init();

    FURI_LOG_D(TAG, "Send Ping Packet");
    comm_send_pingpong(as_reader_worker->comm, NfcRelayPacketPing, NfcRelayAsReader);
    NfcRelayPacket* packet = NULL;

    FuriHalNfcTxRxContext tx_rx = {};
    FuriHalNfcDevData dev_data = {};

    while(1) {
        if(!as_reader_worker->running) {
            break;
        }
        if(as_reader_worker->state == AsReaderWorkerStateWaitPong) {
            if(comm_wait_pong(as_reader_worker->comm, NfcRelayAsCard, NfcRelayAsReader)) {
                FURI_LOG_D(TAG, "comm_wait_pong succ, change state");
                as_reader_worker_change_state(as_reader_worker, AsReaderWorkerStateCardSearch);
            } else {
                continue;
            }
        } else if(as_reader_worker->state == AsReaderWorkerStateCardSearch) {
            if(!furi_hal_nfc_detect(&dev_data, 300)) {
                FURI_LOG_I(TAG, "No Card Detected");
                continue;
            }

            FURI_LOG_D(
                TAG, "packet alloc sizeof(FuriHalNfcDevData)=%d", sizeof(FuriHalNfcDevData));
            packet = packet_alloc(NfcRelayPacketNfcDevData, sizeof(FuriHalNfcDevData), &dev_data);
            FURI_LOG_D(TAG, "packet->len: %d", packet->len);
            comm_write_packet(as_reader_worker->comm, packet);
            free(packet);
            packet = NULL;
            as_reader_worker_change_state(as_reader_worker, AsReaderWorkerStateCardFound);
        } else if(as_reader_worker->state == AsReaderWorkerStateCardFound) {
            NfcRelayPacket* recv_packet;
            recv_packet = comm_wait_packet(as_reader_worker->comm, NfcRelayPacketApduReq);
            if(!recv_packet) {
                continue;
            }
            FURI_LOG_D(TAG, "Recv NfcRelayPacketApduReq");
            // drop PCB
            memcpy(tx_rx.tx_data, &recv_packet->buf[1], recv_packet->len - 1);
            tx_rx.tx_bits = (recv_packet->len - 1) * 8;
            if(!furi_hal_nfc_tx_rx(&tx_rx, 300)) {
                FURI_LOG_E(TAG, "furi_hal_nfc_tx_rx failed");
                packet = packet_no_data_alloc(NfcRelayPacketErr);
                comm_write_packet(as_reader_worker->comm, packet);
                free(recv_packet);
                free(packet);
                recv_packet = NULL;
                packet = NULL;
            }
            packet = packet_alloc(NfcRelayPacketApduResp, (tx_rx.rx_bits + 7) / 8, tx_rx.rx_data);
            comm_write_packet(as_reader_worker->comm, packet);
            free(recv_packet);
            free(packet);
            recv_packet = NULL;
            packet = NULL;
        }
    }
    furi_hal_nfc_ll_txrx_off();
    furi_hal_nfc_sleep();

    return 0;
}