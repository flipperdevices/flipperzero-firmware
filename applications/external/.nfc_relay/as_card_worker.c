#include "as_card_worker.h"

#define TAG "AsCardWorker"

void as_card_worker_change_state(AsCardWorker* as_card_worker, AsCardWorkerState state) {
    as_card_worker->state = state;
    if(as_card_worker->callback) as_card_worker->callback(state, as_card_worker->nfc_relay);
}

AsCardWorker* as_card_worker_alloc(NfcRelay* nfc_relay) {
    furi_assert(nfc_relay);
    FURI_LOG_D(TAG, "as_card_worker_alloc");

    AsCardWorker* as_card_worker = malloc(sizeof(AsCardWorker));
    as_card_worker->thread =
        furi_thread_alloc_ex("AsCardWorkerThread", 8192, as_card_worker_task, as_card_worker);
    as_card_worker->running = false;
    as_card_worker->comm = comm_alloc(nfc_relay->config);
    as_card_worker->nfc_relay = nfc_relay;
    as_card_worker->callback = NULL;
    as_card_worker->emu_card_worker = emu_card_worker_alloc();
    return as_card_worker;
}

void as_card_worker_free(AsCardWorker* as_card_worker) {
    furi_assert(as_card_worker);

    comm_free(as_card_worker->comm);
    if(as_card_worker->emu_card_worker) emu_card_worker_free(as_card_worker->emu_card_worker);
    furi_thread_free(as_card_worker->thread);
    free(as_card_worker);
}

void as_card_worker_stop(AsCardWorker* as_card_worker) {
    furi_assert(as_card_worker);

    as_card_worker->running = false;
    comm_deinit(as_card_worker->comm);
    if(as_card_worker->emu_card_worker) emu_card_worker_stop(as_card_worker->emu_card_worker);
    as_card_worker->callback = NULL;

    furi_thread_join(as_card_worker->thread);
}

void as_card_worker_start(AsCardWorker* as_card_worker, AsCardWorkerCallback callback) {
    FURI_LOG_D(TAG, "as_card_worker_start");
    furi_assert(as_card_worker);
    as_card_worker->running = true;

    comm_init(as_card_worker->comm);
    as_card_worker->callback = callback;
    furi_thread_start(as_card_worker->thread);
}

// TODO: NfcRelayPakcetErr
int32_t as_card_worker_task(void* context) {
    furi_assert(context);
    AsCardWorker* as_card_worker = context;
    NfcRelayPacket* recv_packet = NULL;
    FuriHalNfcDevData dev_data = {};
    uint8_t apdu_buf[256];

    FURI_LOG_D(TAG, "Send Ping Packet");
    comm_send_pingpong(as_card_worker->comm, NfcRelayPacketPing, NfcRelayAsCard);

    while(1) {
        if(!as_card_worker->running) break;
        if(as_card_worker->state == AsCardWorkerStateWaitPong) {
            if(comm_wait_pong(as_card_worker->comm, NfcRelayAsReader, NfcRelayAsCard)) {
                FURI_LOG_D(TAG, "comm_wait_pong succ, change state");
                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitNfcDevData);
            } else {
                continue;
            }
        } else if(as_card_worker->state == AsCardWorkerStateWaitNfcDevData) {
            recv_packet = comm_wait_packet(as_card_worker->comm, NfcRelayPacketNfcDevData);
            if(!recv_packet) continue;
            if(recv_packet->len == sizeof(FuriHalNfcDevData)) {
                memcpy(&dev_data, &recv_packet->buf, recv_packet->len);
                free(recv_packet);
                recv_packet = NULL;
                FURI_LOG_D(TAG, "Start Emulating");
                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitApduReq);
                if(dev_data.uid_len > 7) {
                    FURI_LOG_E(TAG, "UID Length > 7, ST25R3916 not supported??");
                    dev_data.uid_len = 7;
                }
                emu_card_worker_start(as_card_worker->emu_card_worker, &dev_data);
                FURI_LOG_D(TAG, "Started Emulating");
            } else {
                FURI_LOG_D(TAG, "Recv NfcDevData with wrong size");
                free(recv_packet);
                recv_packet = NULL;
                // TODO: change state or not?
            }
        } else if(as_card_worker->state == AsCardWorkerStateWaitApduReq) {
            if(as_card_worker->emu_card_worker->state == EmuCardWorkerStateWaitApduResp) {
                uint16_t apdu_len;
                NfcRelayPacket* packet;
                emu_card_worker_get_apdu(as_card_worker->emu_card_worker, &apdu_buf[0], &apdu_len);
                packet = packet_alloc(NfcRelayPacketApduReq, apdu_len, &apdu_buf[0]);
                comm_write_packet(as_card_worker->comm, packet);
                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitApduResp);
                continue;
            }
            recv_packet = comm_read_packet(as_card_worker->comm);
            if(!recv_packet) {
                continue;
            }
            FURI_LOG_E(TAG, "? Recv packet when state is AsCardWorkerStateWaitApduReq");
            free(recv_packet);
            recv_packet = NULL;
        } else if(as_card_worker->state == AsCardWorkerStateWaitApduResp) {
            if(as_card_worker->emu_card_worker->state == EmuCardWorkerStateWaitApduReq) {
                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitApduReq);
            }
            recv_packet = comm_wait_packet(as_card_worker->comm, NfcRelayPacketApduResp);
            if(!recv_packet) {
                continue;
            }
            emu_card_worker_set_apdu(
                as_card_worker->emu_card_worker, 1, &recv_packet->buf[0], recv_packet->len);
            as_card_worker->emu_card_worker->state = EmuCardWorkerStateGetApduResp;
            FURI_LOG_D(
                TAG,
                "Recv NfcRelayPacketApduResp, change emu_card_worker state to EmuCardWorkerStateGetApduResp");
            free(recv_packet);
            recv_packet = NULL;
        } else {
            recv_packet = comm_read_packet(as_card_worker->comm);
            if(!recv_packet) {
                continue;
            }
            FURI_LOG_E(TAG, "Not Yet");
            free(recv_packet);
            recv_packet = NULL;
        }
    }
    furi_hal_nfc_ll_txrx_off();
    furi_hal_nfc_sleep();

    return 0;
}